/**
 * Creates minimal required directories and files if they don't already exist
 * This is used for Vercel deployment to ensure all needed directories are present
 */

const fs = require('fs');
const path = require('path');

console.log('Starting structure verification...');
console.log(`Current directory: ${__dirname}`);
console.log(`Parent directory: ${path.dirname(__dirname)}`);

// Use absolute paths based on the script's directory
const baseDir = path.dirname(__dirname); // wiki directory

// For Vercel deployment, make sure we know where the views directory should be
console.log('Checking environment paths:');
console.log(`__dirname: ${__dirname}`);
console.log(`baseDir: ${baseDir}`);
console.log(`Current working directory: ${process.cwd()}`);
console.log(`Expected views path: ${path.join(baseDir, 'views')}`);
console.log(`Node environment: ${process.env.NODE_ENV || 'development'}`);

// Check and create directories if they don't exist
const requiredDirs = [
  path.join(baseDir, 'pages'),
  path.join(baseDir, 'pages/packages'),
  path.join(baseDir, 'pages/tutorials'),
  path.join(baseDir, 'views'),
  path.join(baseDir, 'static'),
];

console.log('Checking required directories:', requiredDirs);

requiredDirs.forEach(dir => {
  console.log(`Checking directory: ${dir}`);
  if (!fs.existsSync(dir)) {
    console.log(`Creating directory: ${dir}`);
    fs.mkdirSync(dir, { recursive: true });
  }
});

// Create a basic home.md file if it doesn't exist
const homeMdPath = path.join(baseDir, 'pages', 'home.md');
if (!fs.existsSync(homeMdPath)) {
  console.log(`Creating basic home.md at ${homeMdPath}`);
  fs.writeFileSync(homeMdPath, `# Welcome to KPMWiki\n\nThis is the official documentation for KPM - the Kindle Package Manager.\n\n## Getting Started\n\nMore content will be generated soon.`);
}

// Create a basic error.md file if it doesn't exist
const errorMdPath = path.join(baseDir, 'views', 'error.md');
if (!fs.existsSync(errorMdPath)) {
  console.log(`Creating basic error.md at ${errorMdPath}`);
  fs.writeFileSync(errorMdPath, `# Page Not Found\n\nSorry, the page you are looking for does not exist.\n\n[Return to Home](/)`);
}

// Create page.ejs if it doesn't exist
const pageEjsPath = path.join(baseDir, 'views', 'page.ejs');
if (!fs.existsSync(pageEjsPath)) {
  console.log(`Creating basic page.ejs at ${pageEjsPath}`);
  fs.writeFileSync(pageEjsPath, `<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title><%= title %> | KPM Wiki</title>
  <link rel="stylesheet" href="/static/base.css">
</head>
<body>
  <div class="container">
    <div class="content">
      <%- md %>
    </div>
  </div>
</body>
</html>`);
}

// For Vercel serverless environment, create symlinks to ensure views are found
// This is needed because Vercel looks for views in /var/task/views but our app has them in /var/task/wiki/views
try {
  // Check if we're in the Vercel environment by looking at the directory structure
  const taskDir = path.resolve('/var/task');
  const vercelEnv = fs.existsSync(taskDir);
  
  if (vercelEnv) {
    console.log('Detected Vercel environment, creating symlinks...');
    
    // Create symlinks in /var/task pointing to our app directories if they don't exist
    const targetDirs = ['views', 'pages', 'static'];
    targetDirs.forEach(dir => {
      const sourcePath = path.join(baseDir, dir);
      const targetPath = path.join(taskDir, dir);
      
      // Only create if source exists and target doesn't
      if (fs.existsSync(sourcePath) && !fs.existsSync(targetPath)) {
        try {
          // Create directory symlink
          fs.symlinkSync(sourcePath, targetPath, 'dir');
          console.log(`Created symlink: ${targetPath} -> ${sourcePath}`);
        } catch (err) {
          console.warn(`Warning: Failed to create symlink for ${dir}: ${err.message}`);
        }
      }
    });
  }
} catch (err) {
  console.warn(`Warning: Error checking for Vercel environment: ${err.message}`);
}

console.log('Verification of required directories and files completed');
