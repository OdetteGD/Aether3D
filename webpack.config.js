const path = require("node:path");

const {
  CleanWebpackPlugin
} = require("clean-webpack-plugin");

const JavaScriptObfuscator = require("webpack-obfuscator");



module.exports = {

  mode: "production",



  entry: {
    aether3d: path.resolve(
      __dirname,
      "src/index.js"
    )
  },



  output: {
    path: path.resolve(
      __dirname,
      "web-dist"
    ),



    filename: "aether3d.bundle.js",



    clean: true
  },



  resolve: {
    extensions: [
      ".js",
      ".json"
    ]
  },



  module: {
    rules: [

      {
        test: /\\.js$/,

        exclude: /node_modules/,



        use: {
          loader: "babel-loader",



          options: {
            configFile: path.resolve(
              __dirname,
              "babel.config.json"
            )
          }
        }
      },



      {
        test: /\\.css$/i,

        use: [
          "style-loader",
          "css-loader"
        ]
      }

    ]
  },



  optimization: {
    minimize: true,



    moduleIds: "deterministic",



    chunkIds: "deterministic"
  },



  devtool: false,



  performance: {
    hints: false
  },



  plugins: [

    new CleanWebpackPlugin(),



    new JavaScriptObfuscator(
      {
        compact: true,



        controlFlowFlattening: true,

        controlFlowFlatteningThreshold: 0.75,



        deadCodeInjection: true,

        deadCodeInjectionThreshold: 0.4,



        debugProtection: true,

        debugProtectionInterval: 4000,



        disableConsoleOutput: true,



        identifierNamesGenerator: "hexadecimal",



        numbersToExpressions: true,



        renameGlobals: false,



        selfDefending: true,



        simplify: true,



        splitStrings: true,

        splitStringsChunkLength: 8,



        stringArray: true,



        stringArrayCallsTransform: true,



        stringArrayEncoding: [
          "rc4"
        ],



        stringArrayIndexShift: true,



        stringArrayRotate: true,



        stringArrayShuffle: true,



        stringArrayWrappersCount: 2,



        stringArrayWrappersChainedCalls: true,



        stringArrayWrappersParametersMaxCount: 4,



        stringArrayThreshold: 0.75,



        transformObjectKeys: true,



        unicodeEscapeSequence: false
      },



      []
    )

  ]

};
