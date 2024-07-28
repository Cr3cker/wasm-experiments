const canvas = document.getElementById("gameCanvas");
const ctx = canvas.getContext("2d");

let create_cube, get_coords_cube, get_coords_obs, update_frame, start_jump, end_jump, generate_obstacle;
let newCube, newObs;
let px, py, o_x, o_y;
let lastObsTime = 0;
let treeLink = './tree.png';
let treeImage = new Image();
treeImage.id = "tree";
treeImage.src = treeLink;
treeImage.crossOrigin = true;

Module.onRuntimeInitialized = function() {
    create_cube = Module.cwrap("create_cube", "number");
    generate_obstacle = Module.cwrap("generate_obstacle", "number");
    get_coords_cube = Module.cwrap("get_coords_cube", null, ["number", "number", "number"]);
    get_coords_obs = Module.cwrap("get_coords_obs", null, ["number", "number", "number"]);
    update_frame = Module.cwrap("update_frame", null, ["number", "number", "number"]);
    start_jump = Module.cwrap("start_jump", null, ["number"]);
    end_jump = Module.cwrap("end_jump", null, ["number"]);

    newCube = create_cube();
    newObs = generate_obstacle();
    px = Module._malloc(4);
    py = Module._malloc(4);
    o_x = Module._malloc(4);
    o_y = Module._malloc(4);

    window.requestAnimationFrame(gameLoop);
};

function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.beginPath();
    ctx.moveTo(0, 500);
    ctx.lineTo(1000, 500);
    ctx.stroke();
    get_coords_cube(newCube, px, py);
    get_coords_obs(newObs, o_x, o_y);
    let x = Module.getValue(px, "i32");
    let y = Module.getValue(py, "i32");
    let obs_x = Module.getValue(o_x, "i32");
    let obs_y = Module.getValue(o_y, "i32");

    console.log(obs_x, obs_y, x, y);

    ctx.fillRect(x, y, 50, 50);
    ctx.drawImage(treeImage, obs_x, obs_y, 100, 100);
}

function gameLoop(timestamp) {
    window.addEventListener("keydown", function(event) {
        if (event.code === "Space") {
            start_jump(newCube);
        }
    }, false);
    
    window.addEventListener("keyup", function(event) {
        if (event.code === "Space") {
            end_jump(newCube);
        }
    }, false);

    draw();
    update_frame(newCube, newObs, 1/60);

    if (timestamp - lastObsTime > 3000) {
        newObs = generate_obstacle();
        lastObsTime = timestamp;
    }

    window.requestAnimationFrame(gameLoop);
}