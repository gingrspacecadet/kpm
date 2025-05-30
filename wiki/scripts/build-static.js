const fs = require('fs');
const path = require('path');
const showdown = require('showdown');
const ejs = require('ejs');

// Create a Showdown converter
const converter = new showdown.Converter({
  tables: true,
  simplifiedAutoLink: true,
  strikethrough: true,
  tasklists: true,
  ghCodeBlocks: true,
  emoji: true
});

// Function to get all markdown files in the pages directory
function getAllFiles(dirPath, arrayOfFiles = [], relativePath = '') {
  const files = fs.readdirSync(dirPath);
  files.forEach((file) => {
    const fullPath = path.join(dirPath, file);
    if (fs.statSync(fullPath).isDirectory()) {
      const newRelativePath = path.join(relativePath, file);
      getAllFiles(fullPath, arrayOfFiles, newRelativePath);
    } else {
      if (path.extname(file).toLowerCase() === '.md') {
        arrayOfFiles.push({
          name: path.basename(file, '.md'),
          url: `/${relativePath ? relativePath + '/' : ''}${path.basename(file, '.md')}`,
          path: path.join(relativePath, file),
          folder: relativePath
        });
      }
    }
  });
  return arrayOfFiles;
}

// Build navigation structure from files
function buildNavigation(files) {
  const navigation = {};
  files.forEach(file => {
    const parts = file.folder ? file.folder.split(path.sep) : [];
    let current = navigation;
    parts.forEach(part => {
      if (!part) return;
      if (!current[part]) {
        current[part] = { _files: [] };
      }
      current = current[part];
    });
    current._files.push({
      name: file.name,
      url: file.url
    });
  });
  return navigation;
}

// Create the dist directory
const distDir = path.join(__dirname, '../dist');
if (!fs.existsSync(distDir)) {
  fs.mkdirSync(distDir, { recursive: true });
}

// Copy static assets
const staticDir = path.join(__dirname, '../static');
const distStaticDir = path.join(distDir, 'static');
if (!fs.existsSync(distStaticDir)) {
  fs.mkdirSync(distStaticDir, { recursive: true });
}

// Copy all files from static to dist/static
fs.readdirSync(staticDir).forEach(file => {
  const srcPath = path.join(staticDir, file);
  const destPath = path.join(distStaticDir, file);
  fs.copyFileSync(srcPath, destPath);
  console.log(`Copied ${file} to /static`);
});

// Get all markdown files
const pagesDir = path.join(__dirname, '../pages');
const files = getAllFiles(pagesDir);
const navigation = buildNavigation(files);

// Load EJS templates
const pageTemplate = fs.readFileSync(path.join(__dirname, '../views/page.ejs'), 'utf8');
const directoryTemplate = fs.readFileSync(path.join(__dirname, '../views/directory.ejs'), 'utf8');
const errorTemplate = fs.readFileSync(path.join(__dirname, '../views/error.md'), 'utf8');

// Create 404 page
const errorHtml = converter.makeHtml(errorTemplate);
const error404Page = ejs.render(pageTemplate, {
  title: '404 - Page Not Found',
  content: errorHtml,
  navigation: navigation,
  path: '404',
  toc: []
});
fs.writeFileSync(path.join(distDir, '404.html'), error404Page);
console.log('Created 404.html');

// Create index.html (redirect to home)
const indexHtml = `
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta http-equiv="refresh" content="0;url=/home.html">
  <title>Redirecting to home</title>
</head>
<body>
  <p>Redirecting to <a href="/home.html">home</a>...</p>
</body>
</html>
`;
fs.writeFileSync(path.join(distDir, 'index.html'), indexHtml);
console.log('Created index.html (redirect to home)');

// Process each markdown file and generate HTML
files.forEach(file => {
  try {
    const mdPath = path.join(pagesDir, file.path);
    const mdContent = fs.readFileSync(mdPath, 'utf8');
    const htmlContent = converter.makeHtml(mdContent);
    
    // Create TOC
    const tocRegex = /<h([2-6]).*?id="(.*?)".*?>(.*?)<\/h[2-6]>/g;
    const toc = [];
    let match;
    while ((match = tocRegex.exec(htmlContent)) !== null) {
      toc.push({
        level: parseInt(match[1]),
        id: match[2],
        text: match[3]
      });
    }
    
    // Render the template with the content
    const renderedHtml = ejs.render(pageTemplate, {
      title: file.name,
      content: htmlContent,
      navigation: navigation,
      path: file.url.substring(1),  // Remove leading slash
      toc: toc
    });
    
    // Create output directory if needed
    const outputDir = path.join(distDir, file.folder);
    if (!fs.existsSync(outputDir)) {
      fs.mkdirSync(outputDir, { recursive: true });
    }
    
    // Write the file
    const outputPath = path.join(distDir, `${file.url.substring(1)}.html`);
    fs.writeFileSync(outputPath, renderedHtml);
    console.log(`Created ${file.url.substring(1)}.html`);
    
    // If this is the home page, also create a copy in the root to serve as the landing page
    if (file.name === 'home' && !file.folder) {
      const homeDir = path.join(distDir);
      if (!fs.existsSync(homeDir)) {
        fs.mkdirSync(homeDir, { recursive: true });
      }
      fs.writeFileSync(path.join(homeDir, 'home.html'), renderedHtml);
      console.log('Created home.html (duplicate for landing)');
    }
  } catch (err) {
    console.error(`Error processing ${file.path}:`, err);
  }
});

console.log('Static site generation complete!');
