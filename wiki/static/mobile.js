/**
 * Mobile interaction enhancements for KPMWiki
 */
document.addEventListener('DOMContentLoaded', () => {
  // Enhance table of contents on mobile
  const tocHandler = () => {
    const toc = document.querySelector('.table-of-contents');
    if (toc && window.innerWidth <= 768) {
      const tocHeading = toc.querySelector('h3');
      
      if (tocHeading) {
        // Make TOC collapsible on mobile
        tocHeading.style.cursor = 'pointer';
        const tocContent = document.getElementById('toc');
        
        // Add toggle indicator
        if (!document.getElementById('toc-toggle-indicator')) {
          const indicator = document.createElement('span');
          indicator.id = 'toc-toggle-indicator';
          indicator.innerHTML = ' <i class="fas fa-chevron-down"></i>';
          indicator.style.float = 'right';
          tocHeading.appendChild(indicator);
        }
        
        // Add toggle functionality
        tocHeading.addEventListener('click', () => {
          const isHidden = tocContent.style.display === 'none';
          tocContent.style.display = isHidden ? 'block' : 'none';
          
          const indicator = document.getElementById('toc-toggle-indicator');
          if (indicator) {
            indicator.innerHTML = isHidden ? 
              ' <i class="fas fa-chevron-up"></i>' : 
              ' <i class="fas fa-chevron-down"></i>';
          }
        });
        
        // Initially show the TOC
        tocContent.style.display = 'block';
      }
    }
  };
  
  // Enhanced mobile image handling
  const enhanceImages = () => {
    const articleImages = document.querySelectorAll('.article-content img');
    
    articleImages.forEach(img => {
      // Make images tap-to-zoom on mobile
      if (window.innerWidth <= 768) {
        img.style.cursor = 'pointer';
        
        img.addEventListener('click', () => {
          if (img.classList.contains('zoomed')) {
            img.classList.remove('zoomed');
            img.style.maxWidth = '100%';
            img.style.height = 'auto';
            img.style.zIndex = 'auto';
            img.style.position = 'relative';
          } else {
            img.classList.add('zoomed');
            img.style.maxWidth = 'none';
            img.style.height = 'auto';
            img.style.zIndex = '2000';
            img.style.position = 'relative';
          }
        });
      }
    });
  };
  
  // Handle tables on mobile
  const enhanceTables = () => {
    const tables = document.querySelectorAll('.article-content table');
    
    tables.forEach(table => {
      if (window.innerWidth <= 768) {
        // Add swipe indicator for tables that overflow
        const tableWrapper = document.createElement('div');
        tableWrapper.className = 'table-wrapper';
        tableWrapper.style.position = 'relative';
        tableWrapper.style.width = '100%';
        tableWrapper.style.overflowX = 'auto';
        
        table.parentNode.insertBefore(tableWrapper, table);
        tableWrapper.appendChild(table);
        
        if (table.offsetWidth > tableWrapper.offsetWidth) {
          const swipeIndicator = document.createElement('div');
          swipeIndicator.className = 'swipe-indicator';
          swipeIndicator.innerHTML = '<i class="fas fa-arrows-left-right"></i> Swipe to view';
          swipeIndicator.style.textAlign = 'center';
          swipeIndicator.style.fontSize = '0.8rem';
          swipeIndicator.style.color = '#6c757d';
          swipeIndicator.style.marginBottom = '5px';
          
          tableWrapper.parentNode.insertBefore(swipeIndicator, tableWrapper);
        }
      }
    });
  };
  
  // Execute enhancements
  tocHandler();
  enhanceImages();
  enhanceTables();
  
  // Re-run on window resize
  let resizeTimeout;
  window.addEventListener('resize', () => {
    clearTimeout(resizeTimeout);
    resizeTimeout = setTimeout(() => {
      tocHandler();
      enhanceImages();
      enhanceTables();
    }, 250);
  });
});
