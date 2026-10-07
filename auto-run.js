const fs = require("node:fs");

const path = require("node:path");

const { execFileSync } = require("node:child_process");



const projectRoot = __dirname;



const requiredDirectories = [
  "src",
  "src/native",
  "src/android",
  "scripts",
  ".github",
  ".github/workflows"
];



const requiredFiles = [
  "package.json",
  "webpack.config.js",
  "config.xml",
  "src/index.html",
  "scripts/prepare-web.js",
  "src/native/CMakeLists.txt",
  "src/native/native-lib.cpp",
  "src/android/AndroidManifest.xml",
  "src/android/package.json",
  "src/android/plugin.xml",
  ".github/workflows/build-apk.yml"
];



function runCommand(
  command,
  argumentsList
) {
  try {
    return execFileSync(
      command,
      argumentsList,
      {
        cwd: projectRoot,
        env: process.env,
        stdio: "inherit",
        encoding: "utf8",
        shell: process.platform === "win32"
      }
    );
  } catch (error) {
    const message = error && error.message
      ? error.message
      : String(error);



    throw new Error(
      `${command} ${argumentsList.join(" ")} failed: ${message}`
    );
  }
}



function ensureRepositoryStructure() {
  for (
    const directory of requiredDirectories
  ) {
    fs.mkdirSync(
      path.join(
        projectRoot,
        directory
      ),
      {
        recursive: true
      }
    );
  }



  for (
    const file of requiredFiles
  ) {
    const filePath = path.join(
      projectRoot,
      file
    );



    if (fs.existsSync(filePath)) {
      continue;
    }



    try {
      runCommand(
        "git",
        [
          "restore",
          "--source=HEAD",
          "--",
          file
        ]
      );



      continue;
    } catch (restoreError) {
      console.warn(
        `git restore could not recover ${file}.`
      );
    }



    try {
      runCommand(
        "git",
        [
          "checkout",
          "HEAD",
          "--",
          file
        ]
      );



      continue;
    } catch (checkoutError) {
      console.warn(
        `git checkout could not recover ${file}.`
      );
    }



    fs.writeFileSync(
      filePath,
      "",
      "utf8"
    );



    console.warn(
      `Created fallback file: ${file}`
    );
  }
}



function installDependencies() {
  try {
    runCommand(
      "npm",
      [
        "install"
      ]
    );
  } catch (primaryInstallError) {
    console.error(
      primaryInstallError.message
    );



    runCommand(
      "npm",
      [
        "install",
        "--legacy-peer-deps"
      ]
    );
  }
}



function buildWebApplication() {
  try {
    runCommand(
      "npm",
      [
        "run",
        "build"
      ]
    );
  } catch (primaryBuildError) {
    console.error(
      primaryBuildError.message
    );



    installDependencies();



    runCommand(
      "npm",
      [
        "run",
        "build"
      ]
    );
  }



  const generatedIndexFile = path.join(
    projectRoot,
    "www",
    "index.html"
  );



  const generatedBundleFile = path.join(
    projectRoot,
    "www",
    "dist",
    "aether3d.bundle.js"
  );



  if (!fs.existsSync(
    generatedIndexFile
  )) {
    throw new Error(
      "Build completed without creating www/index.html."
    );
  }



  if (!fs.existsSync(
    generatedBundleFile
  )) {
    throw new Error(
      "Build completed without creating www/dist/aether3d.bundle.js."
    );
  }



  const indexContents = fs.readFileSync(
    generatedIndexFile,
    "utf8"
  );



  if (!indexContents.includes(
    'src="dist/aether3d.bundle.js"'
  )) {
    throw new Error(
      "www/index.html does not reference the production Aether3D bundle."
    );
  }
}



function prepareAndroidPlatform() {
  try {
    runCommand(
      "npx",
      [
        "cordova",
        "platform",
        "add",
        "android@13.0.0",
        "--no-telemetry"
      ]
    );
  } catch (platformAddError) {
    console.error(
      platformAddError.message
    );



    try {
      runCommand(
        "npx",
        [
          "cordova",
          "platform",
          "remove",
          "android",
          "--no-telemetry"
        ]
      );
    } catch (removeError) {
      console.error(
        removeError.message
      );
    }



    runCommand(
      "npx",
      [
        "cordova",
        "platform",
        "add",
        "android@13.0.0",
        "--no-telemetry"
      ]
    );
  }
}



function buildAndroidApplication() {
  try {
    runCommand(
      "npx",
      [
        "cordova",
        "build",
        "android",
        "--debug",
        "--no-telemetry"
      ]
    );
  } catch (primaryAndroidBuildError) {
    console.error(
      primaryAndroidBuildError.message
    );



    runCommand(
      "npx",
      [
        "cordova",
        "prepare",
        "android",
        "--no-telemetry"
      ]
    );



    runCommand(
      "npx",
      [
        "cordova",
        "build",
        "android",
        "--debug",
        "--no-telemetry"
      ]
    );
  }
}



function deployToMainBranch() {
  try {
    runCommand(
      "git",
      [
        "init"
      ]
    );
  } catch (gitInitError) {
    console.warn(
      gitInitError.message
    );
  }



  try {
    runCommand(
      "git",
      [
        "checkout",
        "-B",
        "main"
      ]
    );
  } catch (branchError) {
    console.warn(
      branchError.message
    );
  }



  runCommand(
    "git",
    [
      "add",
      "."
    ]
  );



  try {
    runCommand(
      "git",
      [
        "commit",
        "-m",
        "build: synchronize Aether3D Studio hybrid engine"
      ]
    );
  } catch (commitError) {
    console.warn(
      "No new git commit was created."
    );
  }



  if (
    process.env.AETHER3D_SKIP_PUSH === "1"
  ) {
    return;
  }



  const gitRemotes = execFileSync(
    "git",
    [
      "remote"
    ],
    {
      cwd: projectRoot,
      encoding: "utf8"
    }
  )
    .trim()
    .split(/\\s+/)
    .filter(Boolean);



  if (
    !gitRemotes.includes("origin")
  ) {
    console.warn(
      "No origin remote configured; local commit retained."
    );



    return;
  }



  try {
    runCommand(
      "git",
      [
        "push",
        "-u",
        "origin",
        "main"
      ]
    );
  } catch (primaryPushError) {
    console.error(
      primaryPushError.message
    );



    runCommand(
      "git",
      [
        "pull",
        "--rebase",
        "origin",
        "main"
      ]
    );



    runCommand(
      "git",
      [
        "push",
        "-u",
        "origin",
        "main"
      ]
    );
  }
}



function main() {
  ensureRepositoryStructure();

  installDependencies();

  buildWebApplication();

  prepareAndroidPlatform();

  buildAndroidApplication();

  deployToMainBranch();
}



try {
  main();
} catch (error) {
  console.error(
    "Aether3D self-healing build failed:"
  );

  console.error(error);

  process.exitCode = 1;
}
