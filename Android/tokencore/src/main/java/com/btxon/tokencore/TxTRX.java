package com.btxon.tokencore;

//===========================================
// TRX = Tron 波场币
//===========================================

import org.json.JSONObject;

import java.math.BigInteger;

public class TxTRX extends Tx {
//    static {
//        System.loadLibrary("tokencore");
//    }

    private long _tx;

    public TxTRX() {
    }

    // ...
    /*
    1. TRC10，包括TRX本身
    {
      "block_id": "141A34523C4......1BF2D35",
      "token": "TRX",
      "from": "TPiQeYwqPosuun4DB3UJ5sE7ajnray58nc",
      "to": "TMxbZ97qmYc9sqhKznrbsAeN2B1FMN3B6R",
      "amount": 500
    }

    2. TRC20
    {
      "block_id": "141A34523C4......1BF2D35",
      "contract": "TP52amYqPosutn41B3tJ7sE7ajnray50nd",
      "from": "TPiQeYwqPosuun4DB3UJ5sE7ajnray58nc",
      "to": "TMxbZ97qmYc9sqhKznrbsAeN2B1FMN3B6R",
      "amount": 500
    }

    3. From JSON
    {
      "transaction": {a transaction json object}
    }
    */
//    public int init(final String from_addr, final String to_addr, final BigInteger amount) {
//        _tx = _createTx(from_addr, to_addr, amount.toByteArray());
//        return 0;
//    }
    public int init(final String json_param) {
        _tx = _createTx(json_param);
        return 0;
    }

    /**
     * @param index any
     * @return
     */
    @Override
    public String firmwarePrepareData(int index) {
        return _make_unsigned_tx(_tx);
    }

    private String signResult;

    @Override
    public void firmwareProcessResult(int index, final String signResult) {//FIXME
        this.signResult = signResult;
    }

    @Override
    public String makeSignedTx() {
        return _make_signed_tx(_tx, signResult);
    }

    @Override
    public void close() throws Exception {
        if (_tx != 0) {
            _destroyTx(_tx);
            _tx = 0;
        }
    }

    // 签名，如果是软件签名，则需要传入私钥，如果是硬件签名，则参数wifKey为null
    // 硬件签名时，如果成功，则返回1，表示app应该显示一个输入6位数字的界面，让用户输入认证码
    // 此认证码将在调用 result() 时传入。
    /*
    {
      "key": "11eadf3ae12028ddabc765b16cab6e622080dacf69e0f1eae81ba8ceba5fff15"
    }
    */
    @Override
    public int sign(final String json_param) {
        if (_tx == 0) {
            return -1;
        }

        if (json_param == null) {
            // 使用硬件签名
            return -1;
        } else {
            return _sign(_tx, json_param);
        }
    }

    /*
    {
      "auth_code": "123456"
    }
    */
    public String result(final String json_param) {
        if (_tx == 0) {
            return "";
        }

        if (json_param == null) {
            // 获取软件签名结果
            return _getResult(_tx, "");
        } else {
            // 获取硬件签名结果
            return "";
        }

    }

    @Override
    public String getSignResult() {
        return result(null);
    }

    @Override
    public String getPrivateKeyBySeed(boolean mainNet, String seed) {
        try {
            JSONObject jsonObject = new JSONObject();
            jsonObject.put("main_net", mainNet);
            jsonObject.put("seed", seed);
            return pri_key(jsonObject.toString());
        } catch (Exception e) {
        }
        return null;
    }

    @Override
    public String getPublicKeyByPrivateKey(boolean compress, String privateKey) {
        try {
            JSONObject jsonObject = new JSONObject();
            jsonObject.put("compress", compress);
            jsonObject.put("pri_key", privateKey);
            return pub_key(jsonObject.toString());
        } catch (Exception e) {
        }
        return null;
    }

    @Override
    public String getAddressByPublicKey(boolean mainNet, String publicKey) {
        try {
            JSONObject jsonObject = new JSONObject();
            jsonObject.put("main_net", mainNet);
            jsonObject.put("pub_key", publicKey);
            return address(jsonObject.toString());
        } catch (Exception e) {
        }
        return null;
    }

    /*
        {
          "main_net": true,
          "pub_key": "3423423421231424"
        }
        */
    public String address(final String json_param) {
        return _getAddress(json_param);
    }

    /*
    {
      "seed": "3423423421231424"
    }
    */
    public String pri_key(final String json_param) {
        return _getPriKey(json_param);
    }

    /*
    {
      "compress": false,
      "pri_key": "11eadf3ae12028ddabc765b16cab6e622080dacf69e0f1eae81ba8ceba5fff15"
    }
    */
    public String pub_key(final String json_param) {
        return _getPubKey(json_param);
    }

    //-----------------------------------
    // Native API, see tron.cpp
    //-----------------------------------
    //private native long _createTx(final String from_addr, final String to_addr, final byte[] amount);
    static native long _createTx(final String param);
//    private native long _createTx();

    private native void _destroyTx(final long handler);

    // 如果wifKey为NULL，则表示使用硬件进行签名
    private native int _sign(final long handler, final String param);

    private native String _getResult(final long handler, final String param);

    // 根据公钥计算地址（pubKey为16进制字符串）
    private native String _getAddress(final String param);

    // 根据种子计算HD钱包密钥对
    private native String _getPriKey(final String param);

    // 获取HD钱包地址的公钥
    private native String _getPubKey(final String param);

    private native String _make_unsigned_tx(long handler);

    private native String _make_signed_tx(final long handler, final String signResult);

    public static native String _sign_tx(final String tx, final String priKey);

    public static native String _sign_message(final boolean userTronHeader, final String transaction, final String priKey);
}
