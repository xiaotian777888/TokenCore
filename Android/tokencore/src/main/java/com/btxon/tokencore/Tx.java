package com.btxon.tokencore;

public abstract class Tx implements AutoCloseable {
    static {
        System.loadLibrary("tokencore");
    }

    public Tx() {
    }

    static public boolean initialize() {
        //int v = txGetVersion();
        return true;
    }

    static public int version() {
        return txGetVersion();
    }

//    static public String calcPubKey(final String priKey, final boolean compress) {
//        return _calcPubKeyFromPriKey(priKey, compress);
//    }

    //-----------------------------------
    // Native API, see nativeapi.cpp
    //-----------------------------------
//    static private native String _calcPubKeyFromPriKey(final String priKey, final boolean compress);
    static private native int txGetVersion();

    //通用
    public abstract int init(final String jsonParam);


    //蓝牙
    public abstract String firmwarePrepareData(int index);
    public int getFirmwareProcessIndex(){
        return 0;
    }
    public abstract void firmwareProcessResult(int index, final String signResult);
    public abstract String makeSignedTx();
    public abstract int sign(final String privateKey);
    public abstract String getSignResult();

    public abstract String getPrivateKeyBySeed(boolean mainNet, final String seed);

    public abstract String getPublicKeyByPrivateKey(final boolean compress, final String privateKey);

//    public String getPublicKeyBySeed(boolean mainNet, final boolean compress, final String seed) {
//        return getPublicKeyByPrivateKey(compress, getPrivateKeyBySeed(mainNet, seed));
//    }

    public abstract String getAddressByPublicKey(final boolean mainNet, final String publicKey);

    public static Tx createTxByCoinMajor(int coinMajor) {
        switch (coinMajor) {//FIXME
            case 0:
                return new TxBtc();
            case 1:
                return new TxEth();
            case 2:
                return new TxEOS();
//            case 3:
//                return new TxBHP();
            case 4:
                return new TxTRX();
            default:
                throw new RuntimeException("Unsupported coin major: " + coinMajor);
        }
    }
}
