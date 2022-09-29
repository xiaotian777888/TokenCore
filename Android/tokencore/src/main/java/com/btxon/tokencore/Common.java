package com.btxon.tokencore;

/**
 * Author : yml
 * Created on 2019/1/16.
 */
public class Common {

    public native static String _gen_seed(final int size);

    public native static String _entropy_to_mnemonic(final String entropy);

    public native static String _mnemonic_to_seed(final String mnemonic);

    public native static int _check_mnemonic(final String mnemonic);

}
