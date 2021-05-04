#include "helium.h"

uint32_t pretty_print_hnt(uint8_t *dst, uint64_t n){
	if(n==0) {
		dst[0] ='0';
		dst[1] ='\0';
		return 2;
	}

	uint32_t len = bin2dec(dst, n);
	// this will be used to drop useless 0s
	bool nonzero = false;
	uint32_t written = 0;

	// insert decimal if we are dealing with >1 HNT
	if(len > 8){
		uint32_t i = len - 1;
		// shift out all the values larger than 10^8
		while( i >= (len-8) ) {
			if(dst[i]!='0' || nonzero){
				dst[i+1] = dst[i];
				nonzero = true;
				written++;
			}
			i--;
		}
		// add the decimal if there are non-zeros smaller than 10^8
		if(nonzero){
			dst[ i+1 ] = '.';
			written++;
		}

		written += (len-9);
	} 
	// prepend zeros and add decimal to <1 HNT
	else {
		uint32_t i = 0;
		while( i < len ){
			if(dst[len-i-1]!='0' || nonzero){
				dst[8-i] = dst[len-i-1];
				nonzero = true;
				written++;
			}
			i++;
		}
		while(i <= 8) {
			dst[8-i] = '0';
			i++;
			written++;
		}
		dst[0] = '.';
		written--;

	}
	dst[written+1] ='\0';
	return written;
}


#ifdef HELIUM_TESTNET
#define INDEX 905
#else
#define INDEX 904
#endif

void derive_helium_public_key
(uint32_t account, cx_ecfp_private_key_t *privateKey, cx_ecfp_public_key_t *publicKey) {
	uint8_t keySeed[32];
	static cx_ecfp_private_key_t pk;

    // bip32 path for 44'/904'/n'/0'/0' for Mainnet
    uint32_t bip32Path[] = {44 | 0x80000000, INDEX | 0x80000000, account | 0x80000000, 0x80000000, 0x80000000};

	os_perso_derive_node_bip32_seed_key(HDW_ED25519_SLIP10, CX_CURVE_Ed25519, bip32Path, 5, keySeed, NULL, NULL, 0);

	cx_ecfp_init_private_key(CX_CURVE_Ed25519, keySeed, sizeof(keySeed), &pk);
	if (publicKey) {
		cx_ecfp_init_public_key(CX_CURVE_Ed25519, NULL, 0, publicKey);
		cx_ecfp_generate_pair(CX_CURVE_Ed25519, publicKey, &pk, 1);
	}
	if (privateKey) {
		*privateKey = pk;
	}
	os_memset(keySeed, 0, sizeof(keySeed));
	os_memset(&pk, 0, sizeof(pk));
}

#include "pb.h"
#include <os_io_seproxyhal.h>
#include "../ux/helium_ux.h"


void sign_tx(uint8_t *dst, uint32_t account, const uint8_t *tx, uint16_t length) {
	cx_ecfp_private_key_t privateKey;
    derive_helium_public_key(account, &privateKey, NULL);
	cx_eddsa_sign(&privateKey, CX_RND_RFC6979 | CX_LAST, CX_SHA512, tx, length, NULL, 0, dst, 64, NULL);
	os_memset(&privateKey, 0, sizeof(privateKey));
}




void extract_pubkey_bytes(unsigned char *dst, cx_ecfp_public_key_t *publicKey) {
	for (int i = 0; i < 32; i++) {
		dst[i] = publicKey->W[64 - i];
	}
	if (publicKey->W[32] & 1) {
		dst[31] |= 0x80;
	}
}

void bin2hex(uint8_t *dst, uint8_t *data, uint64_t inlen) {
	static uint8_t const hex[] = "0123456789abcdef";
	for (uint64_t i = 0; i < inlen; i++) {
		dst[2*i+0] = hex[(data[i]>>4) & 0x0F];
		dst[2*i+1] = hex[(data[i]>>0) & 0x0F];
	}
	dst[2*inlen] = '\0';
}

int bin2dec(uint8_t *dst, uint64_t n) {
	if (n == 0) {
		dst[0] = '0';
		dst[1] = '\0';
		return 1;
	}
	// determine final length
	int len = 0;
	for (uint64_t nn = n; nn != 0; nn /= 10) {
		len++;
	}
	// write digits in big-endian order
	for (int i = len-1; i >= 0; i--) {
		dst[i] = (n % 10) + '0';
		n /= 10;
	}
	dst[len] = '\0';
	return len;
}

unsigned char const BASE58ALPHABET[] = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

int btchip_encode_base58(const unsigned char *in, size_t length,
                         unsigned char *out, size_t *outlen) {
  unsigned char buffer[68] = {0};
  size_t i = 0, j;
  size_t startAt, stopAt;
  size_t zeroCount = 0;
  size_t outputSize;

  if (length > MAX_ENC_INPUT_SIZE) {
    return -1;
  }

  while ((zeroCount < length) && (in[zeroCount] == 0)) {
    ++zeroCount;
  }

  outputSize = (length - zeroCount) * 138 / 100 + 1;
  stopAt = outputSize - 1;
  for (startAt = zeroCount; startAt < length; startAt++) {
    int carry = in[startAt];
    for (j = outputSize - 1; (int)j >= 0; j--) {
      carry += 256 * buffer[j];
      buffer[j] = carry % 58;
      carry /= 58;

      if (j <= stopAt - 1 && carry == 0) {
        break;
      }
    }
    stopAt = j;
  }

  j = 0;
  while (j < outputSize && buffer[j] == 0) {
    j += 1;
  }

  if (*outlen < zeroCount + outputSize - j) {
    *outlen = zeroCount + outputSize - j;
    return -1;
  }

  os_memset(out, BASE58ALPHABET[0], zeroCount);

  i = zeroCount;
  while (j < outputSize) {
    out[i++] = BASE58ALPHABET[buffer[j++]];
  }
  *outlen = i;

  return 0;
}

void __attribute__ ((noinline)) get_pubkey_bytes(uint8_t account, uint8_t * out){
	cx_ecfp_public_key_t publicKey;
    derive_helium_public_key(account, NULL, &publicKey);
	extract_pubkey_bytes(out, &publicKey);
}
