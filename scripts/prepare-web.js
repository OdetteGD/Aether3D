const fs = require("node:fs");

const path = require("node:path");



const projectRoot = path.resolve(__dirname, "..");

const sourceWebRoot = path.join(
  projectRoot,
  "src"
);

const destinationWebRoot = path.join(
  projectRoot,
  "www"
);

const sourceIndexFile = path.join(
  sourceWebRoot,
  "index.html"
);

const destinationIndexFile = path.join(
  destinationWebRoot,
  "index.html"
);



function ensureDirectory(directoryPath) {
  fs.mkdirSync(
    directoryPath,
    {
      recursive: true
    }
  );
}



function copyRequiredWebEntryPoint() {
  if (!fs.existsSync(sourceIndexFile)) {
    throw new Error(
      `Required web entry point is missing: ${sourceIndexFile}`
    );
  }



  ensureDirectory(
    destinationWebRoot
  );



  fs.copyFileSync(
    sourceIndexFile,
    destinationIndexFile
  );
}



function copyOptionalStaticAssets() {
  const sourceAssetsDirectory = path.join(
    sourceWebRoot,
    "assets"
  );

  const destinationAssetsDirectory = path.join(
    destinationWebRoot,
    "assets"
  );



  if (!fs.existsSync(sourceAssetsDirectory)) {
    return;
  }



  fs.cpSync(
    sourceAssetsDirectory,
    destinationAssetsDirectory,
    {
      recursive: true
    }
  );
}



function validateCordovaWebRoot() {
  if (!fs.existsSync(destinationIndexFile)) {
    throw new Error(
      "Cordova web root is missing www/index.html."
    );
  }



  const indexContents = fs.readFileSync(
    destinationIndexFile,
    "utf8"
  );



  if (!indexContents.includes(
    'src="dist/aether3d.bundle.js"'
  )) {
    throw new Error(
      "www/index.html does not reference dist/aether3d.bundle.js."
    );
  }
}



function main() {
  copyRequiredWebEntryPoint();

  copyOptionalStaticAssets();

  validateCordovaWebRoot();



  console.log(
    "Aether3D Cordova web root prepared successfully."
  );
}



try {
  main();
} catch (error) {
  console.error(
    "Aether3D web preparation failed."
  );

  console.error(error);

  process.exitCode = 1;
}
