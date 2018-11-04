// �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "JlsIF.hpp"

//---------------------------------------------------------------------
// join_logo_scp
// �Ԃ�l�F
//  0 : ����I��
//  1 : �������s���Ȃ��ŏI��
//  2 : �ݒ�E�t�@�C���G���[
//  3 : ���s�G���[
//---------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int ret;
	try{
		JlsIF jls;
		jls.setArgFull(argc, argv);		// �����ǂݍ���
		ret = jls.runScript();			// ���s
	}
	catch(std::bad_alloc){
		printf("error:bad_alloc\n");
		return 3;
	}
	catch(std::runtime_error){
		printf("error:runtime_error\n");
		return 3;
	}
	catch(std::logic_error){
		printf("error:logic_error\n");
		return 3;
	}
	catch(std::exception){
		printf("error:exception at c++ class\n");
		return 3;
	}
	catch(...){
		// compile with: /EHa
		printf("error:exception (forced exit)\n");
		return 3;
	}

	return ret;
}
