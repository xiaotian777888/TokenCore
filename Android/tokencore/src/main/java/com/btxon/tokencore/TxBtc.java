package com.btxon.tokencore;

//===========================================
// BTX = bitcoin 比特币
//===========================================

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class TxBtc extends Tx {
//    static {
//        System.loadLibrary("tokencore");
//    }

    private long _tx;
    private int signCount;
    private boolean isUsdt;

    public TxBtc() {
    }

    /**
     * @param json_param eg.
     *                   {
     *                   "coin_id":"BTC",
     *                   "transaction_hash":"transaction_hash",
     *                   "from_address":"1FSrEQsFbNUma1eUeK95gpZ4EeTaowsTy6",
     *                   "to_address":"1ArTC5oFM1pcTkuL19dUPq26XoRU2FCH7H",
     *                   "value":"10000",
     *                   "fee":"474",
     *                   "utxo_list":[
     *                   {
     *                   "address":"1FSrEQsFbNUma1eUeK95gpZ4EeTaowsTy6",
     *                   "amount":0.00055201,
     *                   "confirmations":11,
     *                   "satoshis":55201,
     *                   "scriptPubKey":"76a9149e75419513cd2a7698b7f675779e5b0d3c099e2688ac",
     *                   "txid":"78f4e9dc3462c8b28975476ccc03ed88a7bbfa9120ef4d8564370b4aea66ccf7",
     *                   "vout":0
     *                   }
     *                   ]
     *                   }
     * @return 0 if success
     */
    public int init(final String json_param) {
        initSignCountWithJSONStr(json_param);
        _tx = _createTx(json_param);
        return 0;
    }

    private void initSignCountWithJSONStr(String json_param) {
        try {
            JSONObject jsonObject = new JSONObject(json_param);
            isUsdt = "USDT".equals(jsonObject.getString("coin_id"));//FIXME
            JSONArray utxoList = jsonObject.getJSONArray("utxo_list");
            signCount = utxoList.length();
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private int firmwarePrepareDataIndex;

    public String firmwarePrepareData(int index) {
        if (_tx == 0) return null;
        firmwarePrepareDataIndex = index;
        if (isUsdt) {
            return _firmware_prepare_data_usdt(_tx, index);
        } else {
            return _firmware_prepare_data(_tx, index);
        }
    }

    @Override
    public void firmwareProcessResult(int index, final String signResult) {
        if (_tx == 0) return;
        _firmware_process_result(_tx, index, signResult);
    }

    @Override
    public String makeSignedTx() {
        return _make_signed_tx(_tx);
    }

    @Override
    public int getFirmwareProcessIndex() {
        return firmwarePrepareDataIndex;
    }

    @Override
    public int sign(final String privateKey) {
        if (_tx == 0) {
            return -1;
        }
        if (privateKey == null) {
            // 使用硬件签名
            return -1;
        } else {
            if (isUsdt) {
                return _sign_usdt(_tx, privateKey);
            } else {
                return _sign(_tx, privateKey);
            }
        }
    }

    public int sign_usdt(final String privateKey) {
        if (_tx == 0) {
            return -1;
        }
        if (privateKey == null) {
            // 使用硬件签名
            return -1;
        } else {
            return _sign_usdt(_tx, privateKey);
        }
    }

    @Override
    public String getSignResult() {
        if (_tx == 0) return null;
        return _get_sign_result(_tx);
    }

    @Override
    public String getPrivateKeyBySeed(boolean mainNet, String seed) {
        return _get_private_key(seed);
    }

    @Override
    public String getPublicKeyByPrivateKey(boolean compress, String privateKey) {
        return _get_public_key(compress, privateKey);
    }

    @Override
    public String getAddressByPublicKey(boolean mainNet, String publicKey) {
        return _get_address(!mainNet, publicKey);
    }

    public int getBtcLen(){
        return _get_tx_len(_tx);
    }

    public int getUsdtLen(){
        return _get_usdt_len(_tx);
    }

    @Override
    public void close() throws Exception {
        if (_tx != 0) {
            _destroyTx(_tx);
            _tx = 0;
        }
    }

    private static native long _createTx(final String param);

    private native void _destroyTx(final long handler);

    private native String _firmware_prepare_data(final long handler, final int index);

    private native String _firmware_prepare_data_usdt(final long handler, final int index);

    private native void _firmware_process_result(final long handler, int index, final String signResult);

    private native String _make_signed_tx(final long handler);

    private native int _sign(final long handler, final String privateKey);

    private native int _sign_usdt(final long handler, final String privateKey);

    private native String _get_private_key(final String seed);

    private native String _get_sign_result(final long handler);

    private native String _get_public_key(final boolean compress, final String privateKey);

    private native String _get_address(final boolean isTestNet, final String publicKey);

    private native int _get_tx_len(final long handler);

    private native int _get_usdt_len(final long handler);
}
