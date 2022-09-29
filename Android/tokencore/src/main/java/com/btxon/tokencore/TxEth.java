package com.btxon.tokencore;

//===========================================
// ETH = Ethereum 以太
//===========================================

public class TxEth extends Tx {
    static {
        System.loadLibrary("tokencore");
    }

    private long _tx;

    public TxEth() {
    }

    /**
     * @param json_param eg.
     *                   {
     *                   "coin_id":"ETH",
     *                   "transaction_hash":"transaction_hash",
     *                   "from_address":"0xb475AE96A6BF6032ebdaaCd27C5D6556f151bCf4",
     *                   "to_address":"0x7a736b60e2a0e8cdde70df7bcc549a08d9836df4",
     *                   "value":"10000000000000",
     *                   "fee":"105000000000000",
     *                   "nonce":"5",
     *                   "data":"0xf8690584b2d05e008288b8947a736b60e2a0e8cdde70df7bcc549a08d9836df48609184e72a000801ba02dbd63846c31de8113238e173d6f6c1bd2590db7ddb9cc8c41359e7001ca945aa07b2a3617ba752c55f632114382d5a9f7df507a1087b6b280f465609528a5f785",
     *                   "utxo_list":[
     *                   <p>
     *                   ]
     *                   }
     * @return 0 if success
     */
    public int init(final String json_param) {
        _tx = _createTx(json_param);
        return 0;
    }

    /**
     * @param index any
     * @return
     */
    public String firmwarePrepareData(int index) {
        if (_tx == 0) return null;
        return _firmware_prepare_data(_tx);
    }

    @Override
    public void firmwareProcessResult(int index, final String signResult) {
        _firmware_process_result(_tx, signResult);
    }

    @Override
    public String makeSignedTx() {
        return _make_signed_tx(_tx);
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
            return _sign(_tx, privateKey);
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
        return _get_public_key(privateKey);
    }

    @Override
    public String getAddressByPublicKey(boolean mainNet, String publicKey) {
        return _get_address(publicKey);
    }

    public String sign_game(String privateKey,String payload){
        if (_tx == 0) return null;
        if (_make_game_tx(_tx,privateKey,payload) == 0){
            return _get_sign_result(_tx);
        }
        return null;
    }

    @Override
    public void close() {
        if (_tx != 0) {
            _destroyTx(_tx);
            _tx = 0;
        }
    }

    private static native long _createTx(final String param);

    private native void _destroyTx(final long handler);

    private native String _firmware_prepare_data(final long handler);

    private native void _firmware_process_result(final long handler, final String signResult);

    private native String _make_signed_tx(final long handler);

    private native int _sign(final long handler, final String param);

    private native String _get_sign_result(final long handler);

    private native String _get_private_key(final String seed);

    private native String _get_public_key(final String privateKey);

    private native String _get_address(final String publicKey);

    private native int _make_game_tx(final long handler,final String private_key,final String dataStr);
}
