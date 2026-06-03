#include "ncg_main.h"

std::array<uint8_t,4> NcgU32uToBytesBE2( uint32_t inp )
{
	std::array<uint8_t,4> outp;
	outp[3] = static_cast<uint8_t>( (inp >> 0 ) & 0xFF );
	outp[2] = static_cast<uint8_t>( (inp >> 8 ) & 0xFF );
	outp[1] = static_cast<uint8_t>( (inp >> 16) & 0xFF );
	outp[0] = static_cast<uint8_t>( (inp >> 24) & 0xFF );
	return outp;
}
uint32_t NcgBytesBEToU32u2( const std::array<uint8_t,4>& bytes2 )
{
	uint32_t outp = 0;
	outp |= (( static_cast<uint32_t>( bytes2[3] ) ) << 0);
	outp |= (( static_cast<uint32_t>( bytes2[2] ) ) << 8);
	outp |= (( static_cast<uint32_t>( bytes2[1] ) ) << 16);
	outp |= (( static_cast<uint32_t>( bytes2[0] ) ) << 24);
	return outp;
}
std::pair<bool,uint32_t>
NcgGetFileSizeLibc2( std::FILE* fp3, const char* flags2 )
{
	assert( !!fp3 );
	assert( !!flags2 );
	const bool bRewindToZero2 = std::strchr( flags2, 'r' );

	if( !!std::fseek( fp3, 0, SEEK_END ) ){ //if error
		return { 0L, 0,};
	}
	const long nFsize2 = std::ftell( fp3 );
	if( nFsize2 < 0 ){
		return { 0L, 0,};
	}
	const uint32_t uFileSize2 = static_cast<uint32_t>( nFsize2 );
	if( bRewindToZero2 ){
		std::fseek( fp3, 0, SEEK_SET );
	}
	return { 1L, uFileSize2,};
}
/// Calculates checksum given region in the file.
/// Returns 0xFFFFFFFF on error, valid checksum otherwise.
uint32_t NcgGetGamesaveChecksum2(
		uint32_t uBeginOffset2, uint32_t uEndOffset2, std::FILE* fp5 )
{
	assert( !!fp5 );
	assert( uBeginOffset2 < uEndOffset2 );
	const uint32_t uDataSize2 = uEndOffset2 - uBeginOffset2;
	assert( !(uDataSize2 % 4) );
	uint32_t uFloatingChecksum2 = 0;
	if( !!std::fseek( fp5, uBeginOffset2, SEEK_SET ) ){
		printf("NCG: ERROR: fseek failed at position %s.\n", std::to_string(uBeginOffset2).c_str() );
		return 0xFFFFFFFF;
	}
	std::vector<uint8_t> bytes2;
	bytes2.resize( uDataSize2, 0x00 );

	const size_t nReadd = std::fread( bytes2.data(), 1, uDataSize2, fp5 );
	if( nReadd != uDataSize2 ){
		printf("NCG: ERROR: failed reading %s bytes from file.\n", std::to_string(uDataSize2).c_str() );
		return 0xFFFFFFFF;
	}
	for( size_t ii2 = 0; ii2 < uDataSize2; ii2 += 4 ){
		std::array<uint8_t,4> aQuadruple2;
		std::memcpy( aQuadruple2.data(), &bytes2[ii2], 4 );
		uint32_t uQuadruple = NcgBytesBEToU32u2( aQuadruple2 );
		uFloatingChecksum2 += uQuadruple;
	}
	return uFloatingChecksum2;
}

int main( int argc, const char*const* argv )
{
	printf("\n");
	printf("NGII Save File Update Utility by ike9000\n");
	printf("- - - - - - - - - - - - - - - - - - - - -\n");
	printf("\n");
	if( sizeof(void*) == 4 ){
		printf("NCG: 32-bit.\n");
	}
	if( argc <= 1 ){
		printf("NCG: WARN: No input file on the command line. Exiting.\n");
		return 101;
	}
	std::string srInputSaveFileName2 = argv[1];
	printf("NCG: Input game-save file: [%s]\n", srInputSaveFileName2.c_str() );
	std::FILE* fp2 = nullptr;
	std::shared_ptr<void> sbrm2( 0, [&](void*){
		if( !!fp2 ){
			std::fclose( fp2 );
			fp2 = nullptr;
		}
	});
	// "r+" = opens for read and write - contents not destroyed.
	fp2 = std::fopen( srInputSaveFileName2.c_str(), "r+b");
	if( !fp2 ){
		const char* err2 = std::strerror(errno);
		printf("NCG: ERROR: File open failed [%s].\n", (err2 ? err2 : "Unknown error."));
		return 102;
	}
	const uint32_t uSaveFileSize2 = NcgGetFileSizeLibc2( fp2, "r" ).second;
	if( uSaveFileSize2 < NcgGSaveXeniaSize2 ){
		printf("NCG: ERROR: Save file appears to be too small or empty.\n"
			"Detected size: %s\n"
			"Expected size: %s or %s\n",
				std::to_string(uSaveFileSize2).c_str(),
				std::to_string(NcgGSaveXeniaSize2).c_str(),
				std::to_string(NcgGSaveX360Size2).c_str() );
		return 103;
	}
	bool bXeniaMode2 = 1L;
	if( uSaveFileSize2 > NcgGSaveXeniaSize2 ){
		if( uSaveFileSize2 >= NcgGSaveX360Size2 ){
			printf("NCG: Detected Xbox 360 mode gamesave.\n");
			printf("NCG: WARN: the CON header and its data won't be updated.\n");
			if( uSaveFileSize2 > NcgGSaveX360Size2 ){
				printf("NCG: WARN: the 360 gamesave appears too large. Expected file size: %s.\n",
					std::to_string(NcgGSaveX360Size2).c_str() );
			}
			bXeniaMode2 = 0L;
		}else{
			printf("NCG: WARN: the Xenia gamesave appears too large. Expected file size: %s.\n",
				std::to_string(NcgGSaveXeniaSize2).c_str() );
		}
	}else{
		printf("NCG: Detected Xenia mode gamesave.\n");
	}
	const uint32_t uFinalChecksum2 = [&](){
		if( bXeniaMode2 ){
			return NcgGetGamesaveChecksum2(
				NcgGSaveXeniaDataStartOffs2, NcgGSaveXeniaDataEndOffs2, fp2 );
		}else{
			return NcgGetGamesaveChecksum2(
				NcgGSaveX360DataStartOffs2, NcgGSaveX360DataEndOffs2, fp2 );
		}
	}();
	if( uFinalChecksum2 == 0xFFFFFFFF ){
		return 106;
	}
	{
		const uint32_t uChecksumValueOffset2 = ( bXeniaMode2 ?
			NcgGSaveXeniaChecksumValueOffs2 : NcgGSaveX360ChecksumValueOffs2 );
		assert( !!fp2 );
		if( !!std::fseek( fp2, uChecksumValueOffset2, SEEK_SET ) ){
			printf("NCG: ERROR: File set-offset failed.\n");
			return 104;
		}
		std::array<uint8_t,4> aFinalChecksum3;
		aFinalChecksum3 = NcgU32uToBytesBE2( uFinalChecksum2 );
		printf("NCG: Updating checksum value to [%02X %02X %02X %02X]\n", aFinalChecksum3[0], aFinalChecksum3[1], aFinalChecksum3[2], aFinalChecksum3[3] );
		const size_t uNumWritten = std::fwrite( aFinalChecksum3.data(), 1, 4, fp2 );
		if( uNumWritten != 4 ){
			printf("NCG: ERROR: Failed writing checksum to file.\n");
			return 105;
		}
	}
	printf("NCG: Update completed OK.\n");
	return 0;
}
