// Globals
var gameState = {
  targetWord: "",
  currentAttempt: 0,
  maxAttempts: 6,
  gameOver: false,
  gameWon: false,
  letterGrid: [],
  letterKeyboard: {},
  wordLength: 5,
  language: 0, // 0: English, 1: French, 2: German
  message: ""
};

const languageList = ["English", "French", "German"];

const states={
  empty: 0,
  filled: 1,
  correct: 2,
  present: 3,
  absent: 4,
  unused: 5
};
const stateStyles = ["empty", "filled", "correct", "present", "absent", "unused"];

// The logging function
function log(logStuff) {
  var logElement = document.getElementById("log");
  if (logElement) {
    logElement.innerHTML += "<p>" + logStuff + "</p>";
  }
  console.log(logStuff);
}

// Initialize the game
function initGame() {
  log("Initializing KWordle game...");
  
  // Check if wordList is loaded
  if (!wordList || wordList.length === 0) {
    log("Warning: Word list not loaded! Length: " + (wordList ? wordList.length : "undefined"));
  } else {
    log("Word list loaded successfully with " + wordList.length + " languages.");
    if (wordList.length < gameState.language || wordList[gameState.language].length === 0) {
      log("Warning: Word list for the selected language is empty!");
    } else {
      log("Word list for the selected contains "+wordList[gameState.language].length+" words");}
    }
  
  // Reset game state
  gameState.currentAttempt = 0;
  gameState.gameOver = false;
  gameState.gameWon = false;
  gameState.message = "";
  
  // Initialize letter grid
  gameState.letterGrid = [];
  for (var i = 0; i < gameState.maxAttempts; i++) {
    var row = [];
    for (var j = 0; j < gameState.wordLength; j++) {
      row.push({
        letter: "",
        state: states.empty // empty, filled, correct, present, absent
      });
    }
    gameState.letterGrid.push(row);
  }
  
  // Initialize keyboard
  gameState.letterKeyboard = {};
  var letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ".split("");
  for (var i = 0; i < letters.length; i++) {
    gameState.letterKeyboard[letters[i]] = states.unused; // unused, present, correct, absent
  }
  
  // Select a random word
  selectRandomWord();
  
  // Update UI
  updateUI();
  
  // Add indicator key
  updateIndicatorKey();

//  showMessage("Target word selected: " + gameState.targetWord);

}

// Add a key explaining what the symbols mean
function updateIndicatorKey() {
  var keyDiv = document.getElementById('indicator-key');
  if (!keyDiv) {
    keyDiv = document.createElement('div');
    keyDiv.id = 'indicator-key';
    keyDiv.className = 'indicator-key';
    
    // Insert before the keyboard
    var keyboard = document.getElementById('keyboard');
    var gameContainer = document.getElementById('game-container');
    gameContainer.insertBefore(keyDiv, keyboard);
  }
  
  keyDiv.innerHTML = '<span>■ = Correct spot</span> <span>□ = Wrong spot</span> <span>× = Not in word</span>';
}

// Select a random word from the word list
function selectRandomWord() {
  if (!wordList[gameState.language] || wordList[gameState.language].length === 0) {
    log("Error: Word list is empty!");
    return;
  }
  
  var randomIndex = Math.floor(Math.random() * wordList[gameState.language].length);
  gameState.targetWord = wordList[gameState.language][randomIndex];
  log("Target word selected: " + gameState.targetWord);
}

// Handle keyboard input
function handleKeyInput(key) {
  if (gameState.gameOver) {
    return;
  }
  
  key = key.toUpperCase();
  
  if (key === "BACKSPACE" || key === "←") {
    // Clear any existing message when backspace is pressed
    gameState.message = "";
    // Delete the last letter in the current row
    deleteLetter();
  } else if (key === "ENTER") {
    // Submit the current guess
    submitGuess();
  } else if (/^[A-Z]$/.test(key)) {
    // Clear any existing message when a new letter is added
    gameState.message = "";
    // Add letter to the current row
    addLetter(key);
  }
  
  updateUI();
}
// Add a letter to the current attempt
function addLetter(letter) {
  var currentRow = gameState.letterGrid[gameState.currentAttempt];
  for (var i = 0; i < currentRow.length; i++) {
    if (currentRow[i].letter === "") {
      currentRow[i].letter = letter;
      currentRow[i].state = states.filled;
      break;
    }
  }
}

// Delete the last letter from the current attempt
function deleteLetter() {
  var currentRow = gameState.letterGrid[gameState.currentAttempt];
  for (var i = currentRow.length - 1; i >= 0; i--) {
    if (currentRow[i].letter !== "") {
      currentRow[i].letter = "";
      currentRow[i].state = states.empty;
      break;
    }
  }
}

// Check if current row is completely filled
function isRowFilled() {
  var currentRow = gameState.letterGrid[gameState.currentAttempt];
  for (var i = 0; i < currentRow.length; i++) {
    if (currentRow[i].letter === "") {
      return false;
    }
  }
  return true;
}

// Get the current guess as a string
function getCurrentGuess() {
  var currentRow = gameState.letterGrid[gameState.currentAttempt];
  var guess = "";
  for (var i = 0; i < currentRow.length; i++) {
    guess += currentRow[i].letter;
  }
  return guess.toLowerCase();
}

// Check if the word is in the word list
function isValidWord(word) {
  if (!accepted) {
    log("Error: accepted list missing!");
    return false;
  }
  
  for (var i = 0; i < accepted[gameState.language].length; i++) {
    if (accepted[gameState.language][i] === word) {
      return true;
    }
  }
  
  log("Word not found in accepted list: " + word);
  return false;
}

// Submit the current guess
function submitGuess() {
  if (!isRowFilled()) {
    showMessage("Not enough letters");
    return;
  }
  
  var guess = getCurrentGuess();
  
  if (!isValidWord(guess)) {
    showMessage("Not in word list");
    return;
  }
  
  // Check letters against target word
  checkGuess(guess);
  
  // Move to next attempt
  gameState.currentAttempt++;
  
  // Check if game is over
  if (guess === gameState.targetWord) {
    gameState.gameOver = true;
    gameState.gameWon = true;
    showMessage("You won!");
    updateStatistics(true);
  } else if (gameState.currentAttempt >= gameState.maxAttempts) {
    gameState.gameOver = true;
    showMessage("Game over! The word was " + gameState.targetWord);
    updateStatistics(false);
  }
}

// Check the guess against the target word
function checkGuess(guess) {
  var targetLetters = gameState.targetWord.split("");
  var guessLetters = guess.split("");
  var currentRow = gameState.letterGrid[gameState.currentAttempt];
  
  // First pass: mark correct letters
  for (var i = 0; i < guessLetters.length; i++) {
    if (guessLetters[i] === targetLetters[i]) {
      currentRow[i].state = states.correct;
      gameState.letterKeyboard[guessLetters[i].toUpperCase()] = states.correct;
      targetLetters[i] = null; // Mark as used
    }
  }
  
  // Second pass: mark present letters
  for (var i = 0; i < guessLetters.length; i++) {
    if (currentRow[i].state !== states.correct) {
      var letterIndex = targetLetters.indexOf(guessLetters[i]);
      if (letterIndex !== -1) {
        currentRow[i].state = states.present;
        if (gameState.letterKeyboard[guessLetters[i].toUpperCase()] !== states.correct) {
          gameState.letterKeyboard[guessLetters[i].toUpperCase()] = states.present;
        }
        targetLetters[letterIndex] = null; // Mark as used
      } else {
        currentRow[i].state = states.absent;
        if (gameState.letterKeyboard[guessLetters[i].toUpperCase()] !== states.correct && 
            gameState.letterKeyboard[guessLetters[i].toUpperCase()] !== states.present) {
          gameState.letterKeyboard[guessLetters[i].toUpperCase()] = states.absent;
        }
      }
    }
  }
}

// Show message to the user
function showMessage(message) {
  gameState.message = message;
  var messageElement = document.getElementById("message");
  if (messageElement) {
    messageElement.textContent = message;
    messageElement.style.display = "block";
    
    // Hide message after 2 seconds
    setTimeout(function() {
      messageElement.style.display = "none";
    }, 2000);
  }
}

// Update the UI based on the current game state
function updateUI() {
  updateGrid();
  updateKeyboard();
  updateMessage();
}

// Update the letter grid display
function updateGrid() {
  var gridElement = document.getElementById("letter-grid");
  if (!gridElement) return;
  
  gridElement.innerHTML = "";
  
  for (var i = 0; i < gameState.letterGrid.length; i++) {
    var rowDiv = document.createElement("div");
    rowDiv.className = "grid-row";
    
    for (var j = 0; j < gameState.letterGrid[i].length; j++) {
      var cell = gameState.letterGrid[i][j];
      var cellDiv = document.createElement("div");
      cellDiv.className = "grid-cell " + stateStyles[cell.state];
      
      // Improved indicators for Kindle's black and white display
      if (cell.state === states.correct) {
        // Right place - filled square
        cellDiv.innerHTML = cell.letter + "<span class='indicator correct-indicator'>■</span>";
      } else if (cell.state === states.present) {
        // Wrong place - empty square
        cellDiv.innerHTML = cell.letter + "<span class='indicator present-indicator'>□</span>";
      } else {
        cellDiv.textContent = cell.letter;
      }
      
      rowDiv.appendChild(cellDiv);
    }
    
    gridElement.appendChild(rowDiv);
  }
}

// Create a keyboard key button
function createKeyButton(letter) {
  var keyButton = document.createElement("button");
  keyButton.className = "keyboard-key " + stateStyles[gameState.letterKeyboard[letter]];
  
  // Improved indicators for Kindle's black and white display
  if (gameState.letterKeyboard[letter] === states.correct) {
    keyButton.innerHTML = letter + "<span class='key-indicator correct-indicator'>■</span>";
  } else if (gameState.letterKeyboard[letter] === states.present) {
    keyButton.innerHTML = letter + "<span class='key-indicator present-indicator'>□</span>";
  } else if (gameState.letterKeyboard[letter] === states.absent) {
    keyButton.innerHTML = letter + "<span class='key-indicator absent-indicator'>×</span>";
  } else {
    keyButton.textContent = letter;
  }
  
  keyButton.onclick = function() { handleKeyInput(letter); };
  return keyButton;
}

// Update the keyboard display
function updateKeyboard() {
  var keyboardElement = document.getElementById("keyboard");
  if (!keyboardElement) return;
  
  keyboardElement.innerHTML = "";
  
  // Row 1: Q-P
  var row1 = "QWERTYUIOP".split("");
  var keyboardRow1 = createKeyboardRow(row1);
  keyboardElement.appendChild(keyboardRow1);
  
  // Row 2: A-L
  var row2 = "ASDFGHJKL".split("");
  var keyboardRow2 = createKeyboardRow(row2);
  keyboardElement.appendChild(keyboardRow2);
  
  // Row 3: Enter, Z-M, Backspace
  var keyboardRow3 = document.createElement("div");
  keyboardRow3.className = "keyboard-row";
  
  var enterKey = document.createElement("button");
  enterKey.className = "keyboard-key wide-key";
  enterKey.textContent = "✓";
  enterKey.onclick = function() { handleKeyInput("ENTER"); };
  keyboardRow3.appendChild(enterKey);
  
  var row3 = "ZXCVBNM".split("");
  for (var i = 0; i < row3.length; i++) {
    var letter = row3[i];
    var keyButton = createKeyButton(letter);
    keyboardRow3.appendChild(keyButton);
  }
  
  var backspaceKey = document.createElement("button");
  backspaceKey.className = "keyboard-key wide-key";
  backspaceKey.textContent = "←";
  backspaceKey.onclick = function() { handleKeyInput("BACKSPACE"); };
  keyboardRow3.appendChild(backspaceKey);
  
  keyboardElement.appendChild(keyboardRow3);
}

// Create a keyboard row
function createKeyboardRow(letters) {
  var rowDiv = document.createElement("div");
  rowDiv.className = "keyboard-row";
  
  for (var i = 0; i < letters.length; i++) {
    var keyButton = createKeyButton(letters[i]);
    rowDiv.appendChild(keyButton);
  }
  
  return rowDiv;
}

// Create a keyboard key button
function createKeyButton(letter) {
  var keyButton = document.createElement("button");
  keyButton.className = "keyboard-key " + stateStyles[gameState.letterKeyboard[letter]];
  
  // Improved indicators for Kindle's black and white display
  if (gameState.letterKeyboard[letter] === states.correct) {
    keyButton.innerHTML = "<div class='key-with-indicator'>" + letter + "<span class='mini-indicator'>■</span></div>";
  } else if (gameState.letterKeyboard[letter] === states.present) {
    keyButton.innerHTML = "<div class='key-with-indicator'>" + letter + "<span class='mini-indicator'>□</span></div>";
  } else if (gameState.letterKeyboard[letter] === states.absent) {
    keyButton.innerHTML = "<div class='key-with-indicator'>" + letter + "<span class='mini-indicator'>×</span></div>";
  } else {
    keyButton.textContent = letter;
  }
  
  keyButton.onclick = function() { handleKeyInput(letter); };
  return keyButton;
}

// Update message display
function updateMessage() {
  var messageElement = document.getElementById("message");
  if (messageElement && gameState.message) {
    messageElement.textContent = gameState.message;
    messageElement.style.display = "block";
  }
}

// Load statistics from localStorage
function loadStatistics() {
  var stats = localStorage.getItem("kwordle-stats");
  if (stats) {
    return JSON.parse(stats);
  } else {
    return {
      gamesPlayed: 0,
      gamesWon: 0,
      currentStreak: 0,
      maxStreak: 0,
      guessDistribution: [0, 0, 0, 0, 0, 0]
    };
  }
}

// Save statistics to localStorage
function saveStatistics(stats) {
  localStorage.setItem("kwordle-stats", JSON.stringify(stats));
}

// Update statistics after game ends
function updateStatistics(won) {
  var stats = loadStatistics();
  stats.gamesPlayed++;
  
  if (won) {
    stats.gamesWon++;
    stats.currentStreak++;
    stats.guessDistribution[gameState.currentAttempt - 1]++;
    
    if (stats.currentStreak > stats.maxStreak) {
      stats.maxStreak = stats.currentStreak;
    }
  } else {
    stats.currentStreak = 0;
  }
  
  saveStatistics(stats);
}

// Show statistics
function showStatistics() {
  var stats = loadStatistics();
  var winPercentage = stats.gamesPlayed > 0 ? Math.round((stats.gamesWon / stats.gamesPlayed) * 100) : 0;
  
  var statsHtml = "<h2>Statistics</h2>" +
                  "<div class='stats-container'>" +
                  "<div class='stat-box'><div class='stat-number'>" + stats.gamesPlayed + "</div><div class='stat-label'>Played</div></div>" +
                  "<div class='stat-box'><div class='stat-number'>" + winPercentage + "</div><div class='stat-label'>Win %</div></div>" +
                  "<div class='stat-box'><div class='stat-number'>" + stats.currentStreak + "</div><div class='stat-label'>Current Streak</div></div>" +
                  "<div class='stat-box'><div class='stat-number'>" + stats.maxStreak + "</div><div class='stat-label'>Max Streak</div></div>" +
                  "</div>";
  
  statsHtml += "<h2>Guess Distribution</h2><div class='guess-distribution'>";
  
  var maxGuesses = Math.max.apply(null, stats.guessDistribution);
  if (maxGuesses === 0) maxGuesses = 1; // Avoid division by zero
  
  for (var i = 0; i < stats.guessDistribution.length; i++) {
    var guessCount = stats.guessDistribution[i];
    var percentage = Math.max(7, Math.round((guessCount / maxGuesses) * 100));
    
    statsHtml += "<div class='guess-row'><div class='guess-number'>" + (i + 1) + "</div><div class='guess-bar-container'>" +
                "<div class='guess-bar' style='width: " + percentage + "%'>" + guessCount + "</div></div></div>";
  }
  
  statsHtml += "</div>";
  
  var statsModal = document.getElementById("stats-modal");
  var statsContent = document.getElementById("stats-content");
  if (statsModal && statsContent) {
    statsContent.innerHTML = statsHtml;
    statsModal.style.display = "block";
  }
}

// Close the statistics modal
function closeStatistics() {
  var statsModal = document.getElementById("stats-modal");
  if (statsModal) {
    statsModal.style.display = "none";
  }
}

function showCredits() {
  var creditsModal = document.getElementById("credits-modal");
  var creditsContent = document.getElementById("credits-content");
  if (creditsModal && creditsContent) {
    creditsContent.innerHTML = "<h2><font size=\"4\">KWordle</font></h2>" +
      "<p><font size=\"2\">Game by <b><a href=\"https://kurizu.vercel.app/\">kurizu</a></b><br/>" +
      "Illusion engine by <b><a href=\"https://github.com/polish-penguin-dev/\">Penguins184</a></b><br/>"+
      "Additional development by <b><a href=\"https://github.com/kbarni\">kbarni</a></b><br/><br/>"+
      "Word lists by:<br/>&nbsp;&nbsp;- English: <a href=\"https://github.com/seanpatlan/wordle-words\">Sean Patlan</a><br/>&nbsp;&nbsp;- French: <a href=\"https://github.com/scambier/mo-mo-motus\">Simon Cambier</a><br/>&nbsp;&nbsp;- German: <a href=\"https://github.com/octokatherine/word-master\">Katherine Oelsner</a></font></p>";
    creditsModal.style.display = "block";
  }
}
function closeCredits() {
  var creditsModal = document.getElementById("credits-modal");
  if (creditsModal) {
    creditsModal.style.display = "none";
  }
}

// Reset game (start a new game)
function resetGame() {
  initGame();
}

function changeLanguage() {
  var languageButton = document.getElementById("lang");
  if (languageButton) {
    gameState.language = (gameState.language + 1) % languageList.length;
    languageButton.textContent = languageList[gameState.language];
    initGame();
  }
}

// Initialize the game when the page loads
function onPageLoad() {
  // Set up keyboard event listeners
  document.addEventListener("keydown", function(event) {
    var key = event.key.toUpperCase();
    
    if (key === "BACKSPACE" || key === "ENTER" || /^[A-Z]$/.test(key)) {
      handleKeyInput(key);
      event.preventDefault();
    }
  });
  
  // Initialize the game
  initGame();
}