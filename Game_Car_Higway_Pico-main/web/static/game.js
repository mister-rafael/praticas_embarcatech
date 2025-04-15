// Seleciona os elementos principais do DOM
const game = document.getElementById('game');
const player = document.getElementById('player');
const gameOverMsg = document.getElementById('game-over');
const scoreDisplay = document.getElementById('score');
const lastScoreDisplay = document.getElementById('last-score');
const recordScoreDisplay = document.getElementById('record-score');
const speedDisplay = document.getElementById('speed-display');

// Variáveis de estado do jogo
let canRestart = true;
let lane = 'left';
let gameRunning = true;
let score = 0;

// Configurações iniciais de dificuldade
let obstacleSpeed = 5;
let obstacleSpawnRate = 1000;

// Inicia os intervalos de obstáculos e dificuldade
let obstacleInterval = setInterval(spawnObstacle, obstacleSpawnRate);
let difficultyInterval = setInterval(increaseDifficulty, 10000);

// Carrega pontuações anteriores do localStorage
let previousScore = localStorage.getItem('previousScore') || 0;
let recordScore = localStorage.getItem('recordScore') || 0;

// Exibe as pontuações carregadas
lastScoreDisplay.textContent = `Última Pontuação: ${previousScore}`;
recordScoreDisplay.textContent = `Recorde: ${recordScore}`;

// Atualiza a velocidade exibida
updateSpeedDisplay();

/**
 * Atualiza a posição do jogador na pista
 */
function setLane(newLane) {
  if (newLane === lane) return;
  player.classList.remove('left', 'right');
  player.classList.add(newLane);
  lane = newLane;
}

/**
 * Atualiza a exibição da pontuação
 */
function updateScoreDisplay() {
  scoreDisplay.firstChild.textContent = `Pontuação: ${score}`;
}

/**
 * Atualiza o texto de velocidade
 */
function updateSpeedDisplay() {
  const velocidade = obstacleSpeed * 10;
  speedDisplay.textContent = `Velocidade: ${velocidade} km/h`;
}

/**
 * Atualiza a velocidade das linhas da estrada (animação visual)
 */
function updateRoadLineSpeed() {
  const roadLine = document.getElementById('road-line');
  const newDuration = Math.max(0.1, 1 / obstacleSpeed);
  roadLine.style.animationDuration = `${newDuration}s`;
}

/**
 * Reinicia o jogo
 */
function restartGame() {
  // Remove obstáculos existentes
  document.querySelectorAll('.obstacle').forEach(ob => ob.remove());

  // Reseta posição e estado
  player.classList.remove('right');
  player.classList.add('left');
  lane = 'left';
  gameOverMsg.style.display = 'none';
  score = 0;
  gameRunning = true;
  canRestart = true;

  // Atualiza pontuações na tela
  updateScoreDisplay();
  previousScore = localStorage.getItem('previousScore') || 0;
  lastScoreDisplay.textContent = `Última Pontuação: ${previousScore}`;
  recordScore = localStorage.getItem('recordScore') || 0;
  recordScoreDisplay.textContent = `Recorde: ${recordScore}`;

  // Reseta dificuldade
  obstacleSpeed = 5;
  obstacleSpawnRate = 1000;
  updateSpeedDisplay();
  updateRoadLineSpeed();

  // Reinicia os intervalos
  clearInterval(obstacleInterval);
  clearInterval(difficultyInterval);
  obstacleInterval = setInterval(spawnObstacle, obstacleSpawnRate);
  difficultyInterval = setInterval(increaseDifficulty, 10000);
}

/**
 * Aumenta a dificuldade do jogo com o tempo
 */
function increaseDifficulty() {
  if (obstacleSpeed < 15) obstacleSpeed += 1;

  if (obstacleSpawnRate > 300) {
    clearInterval(obstacleInterval);
    obstacleSpawnRate -= 100;
    obstacleInterval = setInterval(spawnObstacle, obstacleSpawnRate);
  }

  updateSpeedDisplay();
  updateRoadLineSpeed();
}

/**
 * Cria e move um novo obstáculo
 */
function spawnObstacle() {
  if (!gameRunning) return;

  const obstacle = document.createElement('div');
  obstacle.classList.add('obstacle');

  // Define a pista do obstáculo aleatoriamente
  const side = Math.random() < 0.5 ? 'left' : 'right';
  obstacle.classList.add(side);
  game.appendChild(obstacle);

  // Posição inicial
  let top = -100;
  obstacle.style.top = top + 'px';

  // Move o obstáculo
  const move = setInterval(() => {
    top += obstacleSpeed;
    obstacle.style.top = top + 'px';

    // Colisão com o jogador
    const obsRect = obstacle.getBoundingClientRect();
    const playerRect = player.getBoundingClientRect();
    const overlapX = obsRect.left < playerRect.right && obsRect.right > playerRect.left;
    const overlapY = obsRect.top < playerRect.bottom && obsRect.bottom > playerRect.top;

    if (overlapX && overlapY) {
      clearInterval(move);
      gameRunning = false;
      clearInterval(obstacleInterval);
      clearInterval(difficultyInterval);

      // Exibe mensagem de fim de jogo
      gameOverMsg.innerHTML = `💥<strong>Batida!</strong> Fim de jogo.<br><br><strong>Pontuação:</strong> ${score}`;
      gameOverMsg.style.display = 'block';

      // Atualiza pontuações no localStorage
      localStorage.setItem('previousScore', score);
      if (score > recordScore) {
        localStorage.setItem('recordScore', score);
        recordScoreDisplay.textContent = `Recorde: ${score}`;
      }

      // Previne reinício imediato
      canRestart = false;
      setTimeout(() => canRestart = true, 2000);
    }

    // Remove obstáculo fora da tela
    if (top > window.innerHeight) {
      game.removeChild(obstacle);
      clearInterval(move);

      if (gameRunning) {
        score += 1;
        updateScoreDisplay();
      }
    }
  }, 10);
}

/**
 * Escuta comandos do teclado
 */
document.addEventListener('keydown', (e) => {
  if (!gameRunning) {
    if (!canRestart) return;
    restartGame();
    return;
  }

  if (e.key === 'ArrowLeft') setLane('left');
  if (e.key === 'ArrowRight') setLane('right');
});

/**
 * Escuta comandos recebidos via WebSocket
 */
const socket = io();
socket.on('command', (data) => {
  if (!gameRunning) {
    if (!canRestart) return;
    restartGame();
    return;
  }

  if (data.action === 'left') setLane('left');
  if (data.action === 'right') setLane('right');
});
