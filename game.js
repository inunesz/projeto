const canvas = document.getElementById("gameCanvas");
const ctx = canvas.getContext("2d");

let gameStarted = false;
let score = 0;
let timeLeft = 60;
let gameOver = false;

const player = {
    x: 50,
    y: 50,
    width: 40,
    height: 40,
    speed: 5,
    lives: 3
};

const exitDoor = {
    x: 740,
    y: 420,
    width: 50,
    height: 70
};

const enemies = [
    {
        x: 600,
        y: 100,
        width: 40,
        height: 40,
        speedX: 3,
        speedY: 0
    },
    {
        x: 200,
        y: 300,
        width: 40,
        height: 40,
        speedX: 0,
        speedY: 2
    },
    {
        x: 500,
        y: 200,
        width: 40,
        height: 40,
        speedX: -2,
        speedY: 2
    }
];

const keysItems = [
    {
        x: 150,
        y: 100,
        width: 25,
        height: 25,
        collected: false
    },
    {
        x: 350,
        y: 250,
        width: 25,
        height: 25,
        collected: false
    },
    {
        x: 650,
        y: 120,
        width: 25,
        height: 25,
        collected: false
    }
];

const walls = [
    { x: 250, y: 0, width: 20, height: 250 },
    { x: 450, y: 150, width: 20, height: 350 },
    { x: 100, y: 350, width: 250, height: 20 },
    { x: 550, y: 50, width: 20, height: 250 }
];

const keys = {};

function startGame() {
    if(!gameStarted) {
        gameStarted = true;
        startTimer();
    }
}

function startTimer() {

    const timer = setInterval(() => {

        if(gameOver) {
            clearInterval(timer);
            return;
        }

        timeLeft--;

        if(timeLeft <= 0) {
            clearInterval(timer);
            endGame(false);
        }

    }, 1000);
}

document.addEventListener("keydown", (e) => {
    keys[e.key.toLowerCase()] = true;
});

document.addEventListener("keyup", (e) => {
    keys[e.key.toLowerCase()] = false;
});

function movePlayer() {

    let oldX = player.x;
    let oldY = player.y;

    if(keys["w"]) player.y -= player.speed;
    if(keys["s"]) player.y += player.speed;
    if(keys["a"]) player.x -= player.speed;
    if(keys["d"]) player.x += player.speed;

    if(player.x < 0) player.x = 0;
    if(player.y < 0) player.y = 0;
    if(player.x + player.width > canvas.width) {
        player.x = canvas.width - player.width;
    }

    if(player.y + player.height > canvas.height) {
        player.y = canvas.height - player.height;
    }

    for(let wall of walls) {
        if(checkCollision(player, wall)) {
            player.x = oldX;
            player.y = oldY;
        }
    }
}

function moveEnemies() {

    enemies.forEach(enemy => {

        enemy.x += enemy.speedX;
        enemy.y += enemy.speedY;

        if(enemy.x <= 0 || enemy.x + enemy.width >= canvas.width) {
            enemy.speedX *= -1;
        }

        if(enemy.y <= 0 || enemy.y + enemy.height >= canvas.height) {
            enemy.speedY *= -1;
        }
    });
}

function checkCollision(a, b) {
    return (
        a.x < b.x + b.width &&
        a.x + a.width > b.x &&
        a.y < b.y + b.height &&
        a.y + a.height > b.y
    );
}

function collectKeys() {

    keysItems.forEach(item => {

        if(!item.collected && checkCollision(player, item)) {
            item.collected = true;
            score += 100;
        }
    });
}

function checkEnemyCollision() {

    enemies.forEach(enemy => {

        if(checkCollision(player, enemy)) {

            player.lives--;

            player.x = 50;
            player.y = 50;

            if(player.lives <= 0) {
                endGame(false);
            }
        }
    });
}

function allKeysCollected() {
    return keysItems.every(item => item.collected);
}

function checkWin() {

    if(allKeysCollected() && checkCollision(player, exitDoor)) {
        endGame(true);
    }
}

function endGame(win) {

    gameOver = true;

    if(win) {
        alert("Você venceu! Pontuação: " + score);
    }
    else {
        alert("Game Over!");
    }

    location.reload();
}

function drawPlayer() {
    ctx.fillStyle = "lime";
    ctx.fillRect(player.x, player.y, player.width, player.height);
}

function drawEnemies() {

    enemies.forEach(enemy => {
        ctx.fillStyle = "red";
        ctx.fillRect(enemy.x, enemy.y, enemy.width, enemy.height);
    });
}

function drawKeys() {

    keysItems.forEach(item => {

        if(!item.collected) {

            ctx.fillStyle = "gold";
            ctx.strokeStyle = "orange";
            ctx.lineWidth = 2;

            // Cabeça da chave
            ctx.beginPath();
            ctx.arc(item.x + 8, item.y + 8, 8, 0, Math.PI * 2);
            ctx.fill();
            ctx.stroke();

            // Buraco da chave
            ctx.fillStyle = "black";
            ctx.beginPath();
            ctx.arc(item.x + 8, item.y + 8, 3, 0, Math.PI * 2);
            ctx.fill();

            // Corpo da chave
            ctx.fillStyle = "gold";
            ctx.fillRect(item.x + 15, item.y + 5, 18, 6);

            // Dentes da chave
            ctx.fillRect(item.x + 24, item.y + 11, 4, 6);
            ctx.fillRect(item.x + 30, item.y + 11, 4, 10);
        }
    });
}

function drawWalls() {

    ctx.fillStyle = "gray";

    walls.forEach(wall => {
        ctx.fillRect(wall.x, wall.y, wall.width, wall.height);
    });
}

function drawExitDoor() {
    ctx.fillStyle = "blue";
    ctx.fillRect(exitDoor.x, exitDoor.y, exitDoor.width, exitDoor.height);
}

function drawHUD() {

    ctx.fillStyle = "white";
    ctx.font = "20px Arial";

    ctx.fillText("Pontuação: " + score, 20, 30);
    ctx.fillText("Vidas: " + player.lives, 20, 60);
    ctx.fillText("Tempo: " + timeLeft, 20, 90);
}

function gameLoop() {

    ctx.clearRect(0, 0, canvas.width, canvas.height);

    if(gameStarted && !gameOver) {

        movePlayer();
        moveEnemies();

        collectKeys();
        checkEnemyCollision();
        checkWin();
    }

    drawWalls();
    drawExitDoor();
    drawKeys();
    drawEnemies();
    drawPlayer();
    drawHUD();

    requestAnimationFrame(gameLoop);
}

gameLoop()
