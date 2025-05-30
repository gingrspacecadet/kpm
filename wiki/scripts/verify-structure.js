/**
 * Creates minimal required directories and files if they don't already exist
 * This is used for Vercel deployment to ensure all needed directories are present
 */

const fs = require('fs');
const path = require('path');

// Check and create directories if they don't exist
const requiredDirs = [
  'pages',
  'pages/packages',
  'pages/tutorials',
  'views',
  'static',
];

requiredDirs.forEach(dir => {
  if (!fs.existsSync(dir)) {
    console.log(`Creating directory: ${dir}`);
    fs.mkdirSync(dir, { recursive: true });
  }
});

// Create a basic home.md file if it doesn't exist
const homeMdPath = path.join('pages', 'home.md');
if (!fs.existsSync(homeMdPath)) {
  console.log('Creating basic home.md');
  fs.writeFileSync(homeMdPath, `# Welcome to KPMWiki\n\nThis is the official documentation for KPM - the Kindle Package Manager.\n\n## Getting Started\n\nMore content will be generated soon.`);
}

// Create a basic error.md file if it doesn't exist
const errorMdPath = path.join('views', 'error.md');
if (!fs.existsSync(errorMdPath)) {
  console.log('Creating basic error.md');
  fs.writeFileSync(errorMdPath, `# Page Not Found\n\nSorry, the page you are looking for does not exist.\n\n[Return to Home](/)`);
}

console.log('Verification of required directories and files completed');
