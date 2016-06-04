all: encoder encoderM decoder decoderM keygen
	
encoder: 
	gcc otp_enc_d.c -o otp_enc_d -w
encoderM: 
	gcc otp_enc.c -o otp_enc -w
decoder:
	gcc otp_dec_d.c -o otp_dec_d -w
decoderM:
	gcc otp_dec.c -o otp_dec -w
keygen:
	gcc keygen.c -o keygen -w