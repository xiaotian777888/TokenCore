package com.btxon.tokencore;

//===========================================
// EOS
//===========================================

import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

public class TxEOS extends Tx {
    private static final String TAG = "TOKENCORE-EOS";
    private long _tx;

    public TxEOS() {
    }

    @Override
    public int init(String jsonParam) {
        _tx = _createTx(jsonParam);
        return 0;
    }

    /**
     * @param index any
     * @return
     */
    @Override
    public String firmwarePrepareData(int index) {//TODO
        return _make_unsigned_tx(_tx);
    }

    private String signResult;

    @Override
    public void firmwareProcessResult(int index, final String signResult) {
        this.signResult = signResult;
    }

    @Override
    public String makeSignedTx() {
        return _make_signed_tx(_tx, signResult);
    }

    public static String getPrivateKey(final String seed) {
        JSONObject jparam = new JSONObject();
        try {
            jparam.put("seed", seed);
        } catch (JSONException e) {
            Log.e(TAG, "----------------->" + e.toString());
        }

        return _getPrivateKey(jparam.toString());
    }

    public static String getPublicKey(final String wifPriKey) {
        return _getPublicKey(wifPriKey);
    }

    public int init(final String code, final String action, final String account, final boolean asOwner, final String binArgs, final String chainInfo) {
        /*
        {
            "owner": false,
            "code": "eosio.token",
            "action: "transfer",
            "account: "apexluna3333",
            "bin_args": "30c61866ead8553520841066ead855350c3000000000000004454f5300000000117465737420454f53207472616e73666572"
            "chain_info": "...."
        }
        */

        JSONObject jparam = new JSONObject();
        try {
            jparam.put("code", code);
            jparam.put("action", action);
            jparam.put("account", account);
            jparam.put("owner", asOwner);
            jparam.put("bin_args", binArgs);
            jparam.put("chain_info", chainInfo);
        } catch (JSONException e) {
            Log.e(TAG, "----------------->" + e.toString());
        }

        _tx = _createTx(jparam.toString());
        return 0;
    }

    public int init(final String chainId, final JSONObject rawJson) {
        /*
        {
            "chain_id": "....",
            "raw_json": {a transaction json object}
        }
        */

        JSONObject jparam = new JSONObject();
        try {
            jparam.put("chain_id", chainId);
            jparam.put("raw_json", rawJson);
        } catch (JSONException e) {
            Log.e(TAG, "----------------->" + e.toString());
        }

        _tx = _createTx(jparam.toString());
        return 0;
    }

    @Override
    public void close() throws Exception {
        if (_tx != 0) {
            _destroy(_tx);
            _tx = 0;
        }
    }


    // EOS special
    public int addAction(final String code, final String action, final String account, final boolean asOwner, final String binArgs) {
        if (_tx == 0) {
            return -1;
        }

        JSONObject jparam = new JSONObject();
        try {
            jparam.put("code", code);
            jparam.put("action", action);
            jparam.put("account", account);
            jparam.put("owner", asOwner);
            jparam.put("bin_args", binArgs);
        } catch (JSONException e) {
            Log.e(TAG, "----------------->" + e.toString());
        }

        return _addAction(_tx, jparam.toString());
    }

    // 软件签名
    // priKey是wif格式，例如 5Hpp8tDpcqdtx9yWGT6HRGomZeoutTEpWnUkRg6KKqrWEASmJF5
    @Override
    public int sign(final String priKey) {
        if (_tx == 0) {
            return -1;
        }
        try {
            JSONObject jsonObject = new JSONObject();
            jsonObject.put("key", priKey);
            return _sign(_tx, jsonObject.toString());
        } catch (Exception e) {
            return -2;
        }
    }
//
//    // 蓝牙硬件签名
//    public int sign() {
//        if (_tx == 0) {
//            return -1;
//        }
//
//        //return EosAPI.tx_sign(nativeTX, wifKey);
//        return -1;
//    }
//
//

    public String result() {
        // 软件签名
        if (_tx == 0) {
            return "";
        }
        return _getResult(_tx, "");
    }

    public String result(final String auth_code) {
        // 硬件签名（需要输入认证码）
        if (_tx == 0) {
            return "";
        }
        return _getResult(_tx, auth_code);
    }

    public static String longToName(final long val) {
        return _longToName(val);
    }

    @Override
    public String getSignResult() {
        return result();//TODO
    }

    @Override
    public String getPrivateKeyBySeed(boolean mainNet, String seed) {
        return getPrivateKey(seed);
    }

    @Override
    public String getPublicKeyByPrivateKey(boolean compress, String privateKey) {
        return getPublicKey(privateKey);
    }

    @Override
    public String getAddressByPublicKey(boolean mainNet, String publicKey) {
        return getPublicKeyByPrivateKey(false, publicKey);//FIXME 1st arg ?
    }

    //-----------------------------------
    // Native API
    //-----------------------------------
    // 根据种子计算HD钱包密钥对
    private native static String _getPrivateKey(final String json_param);

    private native static String _getPublicKey(final String wifPriKey);

    private native static String _longToName(final long val);

    private native static long _createTx(final String json_param);

    private native static void _destroy(final long handler);

    private native static int _addAction(final long handler, final String json_param);

    private native static int _sign(final long handler, final String wifKey);

    public static native String _sign_message(final String raw_json, final String priKey);

    private native static String _getResult(final long handler, final String json_param);

    private native String _make_unsigned_tx(final long handle);

    private native String _make_signed_tx(final long handle, final String signResult);
}
