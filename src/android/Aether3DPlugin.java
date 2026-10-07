package com.aether3d.studio;
import org.apache.cordova.CallbackContext;
import org.apache.cordova.CordovaPlugin;
import org.json.JSONArray;
import org.json.JSONException;
public final class Aether3DPlugin extends CordovaPlugin{@Override public boolean execute(String action,JSONArray args,CallbackContext callbackContext)throws JSONException{if("getEngineVersion".equals(action)){callbackContext.success(Aether3DNativeBridge.getEngineVersion());return true;}if("fastInverseSquareRoot".equals(action)){if(args.length()<1){callbackContext.error("A numeric value is required");return true;}callbackContext.success(String.valueOf(Aether3DNativeBridge.fastInverseSquareRoot((float)args.getDouble(0))));return true;}return false;}}
