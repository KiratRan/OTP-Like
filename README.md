# OTP-Like

This repository contains five programs used to implement a one-time pad-like system.

otp_enc handles the initial plaintext.
### Syntax for otp_enc
>otp_enc *plaintext key port*

otp_enc_d runs in the background as a daemon and once it verifies communication with otp_enc, otp_enc_d sends back the encrypted ciphertext of the received plaintext.
### Syntax for otp_enc
>otp_enc_d *listening_port*

otp_dec and otp_dec_d function in the same way except for the fact that they handle the decryption.

keygen creates the keyfile used by the encryption and decryption files.
### Syntax for keygen
>keygen *keylength*

The files can be compiled using bash and utilizing the compileall script.
Once compiled the files can be tested using the testingscript in the following way
>./testingscript *PORT1 PORT2 > mytestresults 2>&1*
