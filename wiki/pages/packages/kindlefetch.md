# kindlefetch

## Overview
KindleFetch is a system information tool similar to the popular neofetch utility, designed specifically for Kindle devices.

## Features
- Displays Kindle hardware information
- Shows installed firmware version
- Presents battery status
- Displays storage usage
- Shows uptime and other system metrics

## Installation
Install using KPM with:
```bash
kpm -S kindlefetch
```

## Usage
After installation, you can run KindleFetch from the KTerm terminal:
```bash
kindlefetch
```

## Example Output
```
       _,met$$$$$gg.          
    ,g$$$$$$$$$$$$$$$P.       User: root
  ,g$$P"     """Y$$.".        Model: Kindle Paperwhite 4
 ,$$P'              `$$$.     OS: Kindle OS 5.13.7
',$$P       ,ggs.     `$$b:   Kernel: 4.9.119
`d$$'     ,$P"'   .    $$$    Battery: 84%
 $$P      d$'     ,    $$P    Storage: 2.3GB/8GB used
 $$:      $$.   -    ,d$$'    Uptime: 3 days, 7 hours
 $$;      Y$b._   _,d$P'      
 Y$$.    `.`"Y$$$$P"'         
 `$$b      "-.__              
  `Y$$                        
   `Y$$.                      
     `$$b.                    
       `Y$$b.                 
          `"Y$b._             
```

## Uninstallation
To remove kindlefetch, use:
```bash
kpm -R kindlefetch
```

## Source
This package is based on the [KindleFetch](https://justrals.github.io/KindleFetch/) project.
