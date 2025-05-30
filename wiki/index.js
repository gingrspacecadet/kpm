const express = require('express'),
  app = express(),
  fs = require('fs'),
  path = require('path'),
  showdown = require('showdown'),
  converter = new showdown.Converter({
    tables: true,
    tasklists: true,
    strikethrough: true,
    emoji: true,
    highlight: true
  })

// Set up middleware
app.set('view engine', 'ejs')
app.use(express.json())
app.use(express.urlencoded({ extended: true }))
app.use('/static', express.static('static'))

// Handle favicon request
app.get('/favicon.ico', (req, res) => {
  res.end()
})

// Function to recursively get all files within directories
function getAllFiles(dirPath, arrayOfFiles = [], relativePath = '') {
  // Check if directory exists
  if (!fs.existsSync(dirPath)) {
    console.warn(`Directory not found: ${dirPath}`);
    return arrayOfFiles;
  }

  const files = fs.readdirSync(dirPath)

  files.forEach((file) => {
    const fullPath = path.join(dirPath, file)
    
    if (fs.statSync(fullPath).isDirectory()) {
      // Recursively get files from subdirectories
      const newRelativePath = path.join(relativePath, file)
      getAllFiles(fullPath, arrayOfFiles, newRelativePath)
    } else {
      // Only include markdown files
      if (path.extname(file).toLowerCase() === '.md') {
        const filePath = path.join(relativePath, file)
        const displayName = path.basename(file, '.md')
        arrayOfFiles.push({
          name: displayName,
          url: `/${relativePath ? relativePath + '/' : ''}${displayName}`,
          path: filePath,
          folder: relativePath
        })
      }
    }
  })

  return arrayOfFiles
}

// Function to build a nested navigation structure
function buildNavigation(files) {
  const navigation = { _files: [] }
  
  files.forEach(file => {
    const parts = file.folder ? file.folder.split(path.sep) : []
    let current = navigation
    
    parts.forEach(part => {
      if (!part) return
      if (!current[part]) {
        current[part] = { _files: [] }
      }
      current = current[part]
    })
    
    if (!current._files) {
      current._files = []
    }
    
    current._files.push({
      name: file.name,
      url: file.url
    })
  })
  
  return navigation
}

// Error handling
function error(res, status = 404) {
  try {
    let md = fs.readFileSync(`./views/error.md`).toString()
    let files = []
    
    try {
      files = getAllFiles('./pages')
    } catch (e) {
      console.error('Error getting files for error page:', e)
    }
    
    res.status(status).render('page', {
      md: converter.makeHtml(md),
      dir: status === 404 ? '404' : 'Error',
      title: status === 404 ? '404' : 'Error',
      files: files,
      navigation: buildNavigation(files),
      path: status === 404 ? '404' : 'error'
    })
  } catch (err) {
    // Fallback error handling if rendering fails
    res.status(status).send(`
      <html>
        <head>
          <title>Error ${status}</title>
          <style>
            body { font-family: sans-serif; padding: 2rem; }
            h1 { color: #e53e3e; }
          </style>
        </head>
        <body>
          <h1>Error ${status}</h1>
          <p>Sorry, something went wrong. Please try again later.</p>
          <p><a href="/">Go to homepage</a></p>
        </body>
      </html>
    `)
  }
}

// Homepage
app.get('/', (req, res) => {
  res.redirect('/home')
})

// Search functionality
app.get('/search', (req, res) => {
  const query = req.query.q?.toLowerCase()
  const results = []
  
  if (query) {
    const allFiles = getAllFiles('./pages')
    
    allFiles.forEach(file => {
      try {
        const fullPath = path.join('./pages', file.path)
        const content = fs.readFileSync(fullPath, 'utf8')
        
        if (content.toLowerCase().includes(query) || file.name.toLowerCase().includes(query)) {
          // Find the context of the search term
          const lines = content.split('\n')
          let contextLines = []
          
          for (let i = 0; i < lines.length; i++) {
            if (lines[i].toLowerCase().includes(query)) {
              const start = Math.max(0, i - 2)
              const end = Math.min(lines.length, i + 3)
              contextLines = lines.slice(start, end)
              break
            }
          }
          
          results.push({
            name: file.name,
            url: file.url,
            context: contextLines.join('\n').substring(0, 200) + '...',
            folder: file.folder
          })
        }
      } catch (err) {
        console.error(`Error searching file ${file.path}:`, err)
      }
    })
  }
  
  res.render('search', {
    results,
    query,
    files: getAllFiles('./pages'),
    navigation: buildNavigation(getAllFiles('./pages'))
  })
})

// Handle wiki pages with folder support
app.get('/:path(*)', (req, res, next) => {
  const requestPath = req.params.path
  
  // Skip if it's a static file request
  if (requestPath.startsWith('static/')) {
    return next()
  }
  
  try {
    // Determine if this is a file in a subdirectory
    const pathParts = requestPath.split('/')
    const fileName = pathParts.pop() // Last part is the file name
    const dirPath = pathParts.length > 0 ? pathParts.join('/') : ''
    
    // Construct full path to the requested markdown file
    let fullPath
    if (dirPath) {
      fullPath = path.join('./pages', dirPath, `${fileName}.md`)
    } else {
      fullPath = path.join('./pages', `${fileName}.md`)
    }
    
    // Read and convert the markdown file
    let md = fs.readFileSync(fullPath).toString()
    md = converter.makeHtml(md)
    
    // Get all files for navigation
    const allFiles = getAllFiles('./pages')
    const navigation = buildNavigation(allFiles)
    
    // Extract title from the first h1/h2/h3 in the markdown
    let title = fileName
    const titleMatch = md.match(/<h[1-3][^>]*>(.*?)<\/h[1-3]>/i)
    if (titleMatch && titleMatch[1]) {
      title = titleMatch[1].replace(/<[^>]*>/g, '') // Remove any HTML tags in the title
    }
    
    res.render('page', {
      md: md,
      dir: requestPath,
      title: title,
      files: allFiles,
      navigation: navigation,
      path: requestPath
    })
  } catch (err) {
    // Check if this might be a directory request
    const dirPath = path.join('./pages', req.params.path)
    if (fs.existsSync(dirPath) && fs.statSync(dirPath).isDirectory()) {
      // Try to find an index.md file in the directory
      const indexPath = path.join(dirPath, 'index.md')
      if (fs.existsSync(indexPath)) {
        return res.redirect(`/${req.params.path}/index`)
      }
      
      // If no index.md, list the directory contents
      const allFiles = getAllFiles('./pages')
      const navigation = buildNavigation(allFiles)
      
      // Filter files to only show those in the current directory
      const dirFiles = allFiles.filter(file => 
        file.folder === req.params.path || file.folder.startsWith(req.params.path + '/')
      )
      
      res.render('directory', {
        dirPath: req.params.path,
        files: allFiles,
        dirFiles: dirFiles,
        navigation: navigation
      })
    } else {
      error(res)
    }
  }
})

// Catch-all route for 404 errors
app.get('*', (req, res) => {
  error(res)
})

const PORT = process.env.PORT || 3000
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`)
})