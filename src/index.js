import * as THREE from "three";

import "./styles.css";



const viewportCanvas = document.getElementById(
  "viewport"
);



const statusElement = document.getElementById(
  "status"
);



const versionElement = document.getElementById(
  "version"
);



const telemetryElement = document.getElementById(
  "telemetry"
);



const scene = new THREE.Scene();



scene.background = new THREE.Color(
  0x07090D
);



const camera = new THREE.PerspectiveCamera(
  55,
  1,
  0.05,
  500
);



camera.position.set(
  5.5,
  4.5,
  6.5
);



camera.lookAt(
  0,
  0,
  0
);



const renderer = new THREE.WebGLRenderer(
  {
    canvas: viewportCanvas,

    antialias: true,

    alpha: false,

    powerPreference: "high-performance"
  }
);



renderer.setPixelRatio(
  Math.min(
    window.devicePixelRatio || 1,
    2
  )
);



renderer.setSize(
  window.innerWidth,
  window.innerHeight,
  false
);



const hemisphereLight = new THREE.HemisphereLight(
  0xFFFFFF,
  0x202030,
  2.0
);



scene.add(
  hemisphereLight
);



const keyLight = new THREE.DirectionalLight(
  0xFFFFFF,
  2.5
);



keyLight.position.set(
  4,
  8,
  5
);



scene.add(
  keyLight
);



const workspaceGrid = new THREE.GridHelper(
  20,
  40,
  0x46506A,
  0x1E2431
);



workspaceGrid.position.y = -1.0;



scene.add(
  workspaceGrid
);



const axesHelper = new THREE.AxesHelper(
  2.0
);



scene.add(
  axesHelper
);



const cube = new THREE.Mesh(
  new THREE.BoxGeometry(
    1.0,
    1.0,
    1.0
  ),

  new THREE.MeshStandardMaterial(
    {
      color: 0xE72998,

      roughness: 0.42,

      metalness: 0.12
    }
  )
);



cube.position.set(
  0,
  0.5,
  0
);



scene.add(
  cube
);



const clock = new THREE.Clock();



let nativePhysicsAvailable = false;

let latestNativeFrame = null;

let lastPhysicsTimestamp = 0;

let lastFrameDelta = 1 / 60;



function resizeViewport() {

  const width = window.innerWidth;

  const height = window.innerHeight;



  camera.aspect = width / height;

  camera.updateProjectionMatrix();



  renderer.setSize(
    width,
    height,
    false
  );

}



window.addEventListener(
  "resize",
  resizeViewport,
  {
    passive: true
  }
);



function parseTransformationTelemetry(
  rawTelemetry
) {

  const positionMatch = rawTelemetry.match(
    /pos=\\((-?[0-9.]+),(-?[0-9.]+),(-?[0-9.]+)\\)/
  );



  const velocityMatch = rawTelemetry.match(
    /vel=\\((-?[0-9.]+),(-?[0-9.]+),(-?[0-9.]+)\\)/
  );



  if (
    !positionMatch ||
    !velocityMatch
  ) {
    return null;
  }



  return {
    position: [
      Number(positionMatch[1]),
      Number(positionMatch[2]),
      Number(positionMatch[3])
    ],

    velocity: [
      Number(velocityMatch[1]),
      Number(velocityMatch[2]),
      Number(velocityMatch[3])
    ]
  };

}



function updateTelemetryDisplay(
  frame
) {

  if (
    !frame ||
    typeof frame.telemetryLine !== "string"
  ) {
    return;
  }



  const parsedTelemetry =
    parseTransformationTelemetry(
      frame.telemetryLine
    );



  if (!parsedTelemetry) {
    return;
  }



  const nativeSystemsTelemetry = [
    frame.telemetryLine,

    `vehicle speed=${Number(
      frame.vehicleSpeed || 0
    ).toFixed(2)} m/s`,

    `engine rpm=${Math.round(
      frame.engineRpm || 0
    )}`,

    `vehicle health=${Math.round(
      frame.vehicleHealth || 0
    )}`,

    `character grounded=${Boolean(
      frame.characterGrounded
    )}`,

    `character speed=${Number(
      frame.characterSpeed || 0
    ).toFixed(2)} m/s`,

    `destructible health=${Math.round(
      frame.destructibleHealth || 0
    )}`,

    `fracture pieces=${Math.round(
      frame.fracturePieces || 0
    )}`
  ];



  telemetryElement.textContent =
    nativeSystemsTelemetry.join(
      "\\n"
    );



  cube.position.set(
    parsedTelemetry.position[0],
    parsedTelemetry.position[1],
    parsedTelemetry.position[2]
  );



  cube.quaternion.set(
    frame.orientation[0],
    frame.orientation[1],
    frame.orientation[2],
    frame.orientation[3]
  );

}



function initializeNativeBridge() {

  if (
    !window.NativePhysicsBridge
  ) {
    statusElement.textContent =
      "Native bridge unavailable; WebGL fallback active.";

    return;
  }



  try {

    const engineVersion =
      window.NativePhysicsBridge.getEngineVersion();



    versionElement.textContent =
      `Physics Core: ${engineVersion}`;



    nativePhysicsAvailable = true;



    statusElement.textContent =
      "Native C++ physics connected.";

  } catch (error) {

    nativePhysicsAvailable = false;



    statusElement.textContent =
      "Native bridge failed; WebGL fallback active.";

  }

}



function stepNativePhysics(
  timestamp
) {

  if (!nativePhysicsAvailable) {
    return;
  }



  if (
    timestamp - lastPhysicsTimestamp < 16
  ) {
    return;
  }



  lastPhysicsTimestamp =
    timestamp;



  lastFrameDelta = Math.min(
    clock.getDelta(),
    0.033
  );



  const request = JSON.stringify(
    {
      dt: lastFrameDelta,

      positions: [
        cube.position.x,
        cube.position.y,
        cube.position.z
      ],

      velocities: latestNativeFrame
        ? latestNativeFrame.velocity
        : [0, 0, 0]
    }
  );



  try {

    const rawFrame =
      window.NativePhysicsBridge.stepSimulation(
        request
      );



    latestNativeFrame =
      JSON.parse(
        rawFrame
      );



    updateTelemetryDisplay(
      latestNativeFrame
    );

  } catch (error) {

    nativePhysicsAvailable = false;



    statusElement.textContent =
      "Native frame failed; WebGL fallback active.";

  }

}



function renderFrame(
  timestamp
) {

  requestAnimationFrame(
    renderFrame
  );



  stepNativePhysics(
    timestamp
  );



  if (!nativePhysicsAvailable) {

    cube.rotation.y +=
      lastFrameDelta * 0.45;

  }



  renderer.render(
    scene,
    camera
  );

}



resizeViewport();



initializeNativeBridge();



requestAnimationFrame(
  renderFrame
);
