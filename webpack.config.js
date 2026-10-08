const path = require("node:path");

const {
  CleanWebpackPlugin
} = require("clean-webpack-plugin");

const CopyWebpackPlugin = require("copy-webpack-plugin");

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
        test: /\.js$/,

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
        test: /\.css$/i,

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



    new CopyWebpackPlugin({

      patterns: [

        {
          from: path.resolve(
            __dirname,
            "src/index.html"
          ),

          to: "index.html"
        }
      ]
    }),





  ]

};
