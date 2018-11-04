//
// ������Ǝ��Ԃƃt���[���ʒu�̑��ݕϊ��N���X
//
#include "stdafx.h"

using namespace std;
#include "CnvStrTime.hpp"


//---------------------------------------------------------------------
// �\�z�������ݒ�
//---------------------------------------------------------------------
CnvStrTime::CnvStrTime(){
	m_frate_n = 30000;
	m_frate_d = 1001;
	m_unitsec = 0;
}



//=====================================================================
// �t�@�C�����E�p�X�̕�������
//=====================================================================

//---------------------------------------------------------------------
// �����񂩂�t�@�C���p�X�����ƃt�@�C���������𕪗�
// �ǂݏI������ʒu��Ԃ�l�Ƃ���i���s����-1�j
// ���́F
//   fullpath : �t���p�X��
// �o�́F
//   pathname : �p�X����
//---------------------------------------------------------------------
int CnvStrTime::getStrFilePath(string &pathname, const string &fullname){
	string strTmp;
	return getStrFilePathName(pathname, strTmp, fullname);
}

//---------------------------------------------------------------------
// �����񂩂�t�@�C���p�X�����ƃt�@�C���������𕪗�
// �ǂݏI������ʒu��Ԃ�l�Ƃ���i���s����-1�j
// ���́F
//   fullpath : �t���p�X��
// �o�́F
//   pathname : �p�X����
//   fname    : ���O�ȍ~����
//---------------------------------------------------------------------
int CnvStrTime::getStrFilePathName(string &pathname, string &fname, const string &fullname){
	bool flag_find = false;
	//--- "\"��؂������ ---
	int nloc = (int) fullname.rfind("\\");
	if (nloc >= 0){
		//--- Shift-JIS��2�o�C�g�ڂł͂Ȃ�"\"������ ---
		while(nloc >= 0 && flag_find == false){
			if (nloc == 0){
				flag_find = true;			// �擪��"\"�̃t�@�C����؂�
			}
			else if (isStrSjisSecond(fullname, nloc) == false){
				flag_find = true;			// �O��������̂Q�o�C�g�����łȂ��̂ŋ�؂�
			}
			else{							// �Q�o�C�g�����̈ꕔ�������玟������
				nloc = (int) fullname.rfind("\\", nloc-1);
			}
		}
	}
	//--- "/"��؂������ ---
	int nloc_sl = (int) fullname.rfind("/");
	if (nloc_sl >= 0){
		if (flag_find == false || nloc < nloc_sl){
			flag_find = true;
			nloc = nloc_sl;
		}
	}
	if (flag_find){
		pathname = fullname.substr(0, nloc+1);
		fname    = fullname.substr(nloc+1);
	}
	else{
		pathname = "";
		fname = fullname;
		nloc = -1;
	}
	return nloc;
}



//=====================================================================
//  ���Ԃƃt���[���ʒu�̕ϊ�
//  ���ӓ_�F�t���[���ʒu����̕ϊ��͐擪�t���[����0�Ƃ�����Έʒu�Ŏw�肷��悤�ɂ��Ă���
//=====================================================================

//---------------------------------------------------------------------
// �~���b���t���[�����ɕϊ�
//---------------------------------------------------------------------
int CnvStrTime::getFrmFromMsec(int msec){
	int r = ((((long long)abs(msec) * m_frate_n) + (m_frate_d*1000/2)) / (m_frate_d*1000));
	return (msec >= 0)? r : -r;
}

//---------------------------------------------------------------------
// �t���[�����ɑΉ�����~���b�����擾
//---------------------------------------------------------------------
int CnvStrTime::getMsecFromFrm(int frm){
	int r = (((long long)abs(frm) * m_frate_d * 1000 + (m_frate_n/2)) / m_frate_n);
	return (frm >= 0)? r : -r;
}

//---------------------------------------------------------------------
// �~���b����x�t���[�����Ɋ��Z������~���b�ɕϊ��i�t���[���P�ʂɂȂ�悤�Ɂj
//---------------------------------------------------------------------
int CnvStrTime::getMsecAlignFromMsec(int msec){
	int frm = getFrmFromMsec(msec);
	return getMsecFromFrm(frm);
}

//---------------------------------------------------------------------
// �~���b����x�t���[�����Ɋ��Z��������������ă~���b�ɕϊ�
//---------------------------------------------------------------------
int CnvStrTime::getMsecAdjustFrmFromMsec(int msec, int frm){
	int frm_new = getFrmFromMsec(msec) + frm;
	return getMsecFromFrm(frm_new);
}

//---------------------------------------------------------------------
// �~���b��b���ɕϊ�
//---------------------------------------------------------------------
int CnvStrTime::getSecFromMsec(int msec){
	if (msec < 0){
		return -1 * ((-msec + 500) / 1000);
	}
	return ((msec + 500) / 1000);
}

//---------------------------------------------------------------------
// �t���[�����[�g�ύX�֐��i���g�p�j
//---------------------------------------------------------------------
int CnvStrTime::changeFrameRate(int n, int d){
	m_frate_n = n;
	m_frate_d = d;
	return 1;
}

//---------------------------------------------------------------------
// �������͎��̒P�ʐݒ�
//---------------------------------------------------------------------
int CnvStrTime::changeUnitSec(int n){
	m_unitsec = n;
	return 1;
}



//=====================================================================
// �����񂩂琔�l���擾
// [��{����]
//   �����񂩂�P�P���ǂݍ��ݐ��l�Ƃ��ďo��
//   src������̈ʒupos����P�P���ǂݍ��݁A���l���o��
//   �ǂݏI������ʒu��Ԃ�l�Ƃ���i���s����-1�j
// ���́F
//   cstr : ������
//   pos  : �F���J�n�ʒu
// �o�́F
//   �Ԃ�l�F �ǂݏI������ʒu��Ԃ�l�Ƃ���i���s����-1�j
//   val    : ���ʐ��l
//=====================================================================

//---------------------------------------------------------------------
// �P�P���ǂݍ��ݐ��l�Ƃ��ďo�́i���l�ȊO������΂����ŏI���j
//---------------------------------------------------------------------
int CnvStrTime::getStrValNumHead(int &val, const string &cstr, int pos){
	// unitsec=2�i�P�ʕϊ����Ȃ��j
	// type=EXNUM�i���l�ȊO������΂����ŏI���j
	return getStrValSubDelimit(val, cstr, pos, 2, DELIMIT_SPACE_EXNUM);
}

//---------------------------------------------------------------------
// �P�P���ǂݍ��ݐ��l�Ƃ��ďo�́i���l�ȊO������Γǂݍ��ݎ��s��Ԃ��j
//---------------------------------------------------------------------
int CnvStrTime::getStrValNum(int &val, const string &cstr, int pos){
	return getStrValSub(val, cstr, pos, 2);		// unitsec=2�i�P�ʕϊ����Ȃ��j
}

//---------------------------------------------------------------------
// �P�P���ǂݍ��ݐ��l�i�~���b�j�Ƃ��ďo�́i���l�ȊO������Γǂݍ��ݎ��s��Ԃ��j
//---------------------------------------------------------------------
int CnvStrTime::getStrValMsec(int &val, const string &cstr, int pos){
	return getStrValSub(val, cstr, pos, m_unitsec);
}

//---------------------------------------------------------------------
// ���l�i�~���b�j��Ԃ����A�������͂͐ݒ�ɂ�����炸�t���[�����Ƃ��Ĉ���
//---------------------------------------------------------------------
int CnvStrTime::getStrValMsecFromFrm(int &val, const string &cstr, int pos){
	return getStrValSub(val, cstr, pos, 0);			// unitsec=0:�������̓t���[����
}

//---------------------------------------------------------------------
// ���l�i�~���b�j��Ԃ����A�}�C�i�X�P�̎��͓��ꈵ���ŕϊ��������̂܂ܕԂ�
//---------------------------------------------------------------------
int CnvStrTime::getStrValMsecM1(int &val, const string &cstr, int pos){
	int posnew = getStrValSub(val, cstr, pos, m_unitsec);
	if ((m_unitsec == 0 && getFrmFromMsec(val) == -1) ||
		(m_unitsec == 1 && val == -1000)){
		if (posnew > 0){
			if ((int)cstr.substr(pos, posnew-pos).find(".") < 0){
				val = -1;
			}
		}
	}
	return posnew;
}

//---------------------------------------------------------------------
// �P�P���ǂݍ��ݐ��l�i�b�j�Ƃ��ďo�́i���l�ȊO������Γǂݍ��ݎ��s��Ԃ��j
//---------------------------------------------------------------------
int CnvStrTime::getStrValSec(int &val, const string &cstr, int pos){
	int tmpval;
	pos = getStrValSub(tmpval, cstr, pos, m_unitsec);
	val = (abs(tmpval) + 500) / 1000;
	if (tmpval < 0){
		val = -val;
	}

	return pos;
}

//---------------------------------------------------------------------
// ���l�i�b�j��Ԃ����A�������͂͐ݒ�ɂ�����炸�b�����͂Ƃ��Ĉ���
//---------------------------------------------------------------------
int CnvStrTime::getStrValSecFromSec(int &val, const string &cstr, int pos){
	int tmpval;
	pos = getStrValSub(tmpval, cstr, pos, 1);			// unitsec=1:�������͕b��
	val = (abs(tmpval) + 500) / 1000;
	if (tmpval < 0){
		val = -val;
	}

	return pos;
}



//=====================================================================
// �����񂩂�P����擾
// [��{����]
//   �����񂩂�P�P���ǂݍ��ݏo��
//   src������̈ʒupos����P�P���ǂݍ���dst�ɏo��
//   �ǂݏI������ʒu��Ԃ�l�Ƃ���i���s����-1�j
// ���́F
//   cstr : ������
//   pos  : �F���J�n�ʒu
// �o�́F
//   �Ԃ�l�F �ǂݏI������ʒu��Ԃ�l�Ƃ���i���s����-1�j
//   dst    : �o�͕�����
//=====================================================================

//---------------------------------------------------------------------
// �����񂩂�X�y�[�X��؂�łP�P���ǂݍ���
//---------------------------------------------------------------------
int CnvStrTime::getStrItem(string &dst, const string &cstr, int pos){
	int st, ed;

	pos = getStrItemRange(st, ed, cstr, pos, DELIMIT_SPACE_QUOTE);
	if (pos >= 0){
		if (ed > st) {
			dst = cstr.substr(st, ed - st);
		}
		else {
			dst.clear();
		}
	}

	return pos;
}

//---------------------------------------------------------------------
// �����񂩂�P�P���ǂݍ��ށi�X�y�[�X�ȊO��","����؂�Ƃ��ĔF���j
//---------------------------------------------------------------------
int CnvStrTime::getStrWord(string &dst, const string &cstr, int pos){
	int st, ed;

	pos = getStrItemRange(st, ed, cstr, pos, DELIMIT_SPACE_COMMA);
	if (pos >= 0){
		dst = cstr.substr(st, ed-st);
	}

	return pos;
}



//=====================================================================
// ���Ԃ𕶎���i�t���[���܂��̓~���b�j�ɕϊ�
//=====================================================================

//---------------------------------------------------------------------
// �t���[�����܂��͎��ԕ\�L�i-1�͂��̂܂܎c���j
//---------------------------------------------------------------------
string CnvStrTime::getStringMsecM1(int msec_val){
	bool type_frm = false;
	if (m_unitsec == 0){
		type_frm = true;
	}
	return getStringMsecM1All(msec_val, type_frm);
}

//---------------------------------------------------------------------
// �t���[���\�L�i-1�͂��̂܂܎c���j
//---------------------------------------------------------------------
string CnvStrTime::getStringFrameMsecM1(int msec_val){
	return getStringMsecM1All(msec_val, true);
}

//---------------------------------------------------------------------
// ���ԕ\�L�i-1�͂��̂܂܎c���j
//---------------------------------------------------------------------
string CnvStrTime::getStringTimeMsecM1(int msec_val){
	return getStringMsecM1All(msec_val, false);
}

//---------------------------------------------------------------------
// ���Ԃ𕶎���i�t���[���܂��͎��ԕ\�L�j�ɕϊ�
//---------------------------------------------------------------------
string CnvStrTime::getStringMsecM1All(int msec_val, bool type_frm){
	string str_val;
	if (msec_val == -1){
		str_val = to_string(-1);
	}
	else if (type_frm){
		int n = getFrmFromMsec(msec_val);
		str_val = to_string(n);
	}
	else{
		int val_abs = (msec_val < 0)? -msec_val : msec_val;
		int val_t = val_abs / 1000;
		int val_h = val_t / 3600;
		int val_m = (val_t / 60) % 60;
		int val_s = val_t % 60;
		int val_x = val_abs % 1000;
		string str_h = getStringZeroRight(val_h, 2);
		string str_m = getStringZeroRight(val_m, 2);
		string str_s = getStringZeroRight(val_s, 2);
		string str_x = getStringZeroRight(val_x, 3);
		str_val = str_h + ":" + str_m + ":" + str_s;
		if (val_x > 0){
			str_val = str_val + "." + str_x;
		}
		if (msec_val < 0){
			str_val = "-" + str_val;
		}
	}
	return str_val;
}


//---------------------------------------------------------------------
// ���l�𕶎���ɕϊ��i���0���߁j
//---------------------------------------------------------------------
string CnvStrTime::getStringZeroRight(int val, int len){
	string str_val = "";
	for(int i=0; i<len-1; i++){
		str_val += "0";
	}
	str_val += to_string(val);
	return str_val.substr( str_val.length()-len );
}




//=====================================================================
//
// �����񏈗��̓����֐�
//
//=====================================================================

//---------------------------------------------------------------------
// �Ώۈʒu���V�t�gJIS��2�o�C�g�ڂ�����
//---------------------------------------------------------------------
bool CnvStrTime::isStrSjisSecond(const string &str, int n){
	//--- �ŏ�����2�o�C�g�F�����Ă����珜�� ---
	for(int i=0; i < (int) str.length(); i++){
		int code = str[n];
		if (code < -128 || code > 255) return false;
	}
	//--- �����ʒu�����ԂɃ`�F�b�N ---
	{
		int i = 0;
		while(i < n){
			if ( isStrSjisMultiByte(str, i) ){
				i++;
			}
			i++;
		}
		if (i != n) return true;
	}
	return false;
}

//---------------------------------------------------------------------
// Shift-JIS�̂Q�o�C�g�����`�F�b�N
// 2�o�C�g��������������1��Ԃ�l�Ƃ���i1�o�C�g������0�j
//---------------------------------------------------------------------
bool CnvStrTime::isStrSjisMultiByte(const string &str, int n){
	int code = str[n];
	if (code < 0){
		code = code & 0xFF;
	}
	else if (code >= 0x80){
		return false;
	}
//printf("code:%x\n", code);
	if ((code >= 0x81 && code <= 0x9F) ||
		(code >= 0xE0 && code <= 0xFC)){		// Shift-JIS 1st-byte
		code = (str[n+1] & 0xFF);
		if ((code >= 0x40 && code <= 0x7E) ||
			(code >= 0x80 && code <= 0xFC)){	// Shift-JIS 2nd-byte
			return true;
		}
	}
	return false;
}


//---------------------------------------------------------------------
// �����񂩂�P�P���ǂݍ��ݐ��l�i�~���b�j�Ƃ��Ċi�[�i���l�ȊO������Γǂݍ��ݎ��s��Ԃ��j
// cstr������̈ʒupos����P�P���ǂݍ��݁A���l��val�ɏo��
// ���́F
//   cstr : ������
//   pos  : �F���J�n�ʒu
//   unitsec : ���������̒P�ʁi0=�t���[����  1=�b��  2=�P�ʕϊ��Ȃ��j
// �o�́F
//   �Ԃ�l�F �ǂݏI������ʒu��Ԃ�l�Ƃ���i���s����-1�j
//   val    : ���l�i�~���b�j
//---------------------------------------------------------------------
int CnvStrTime::getStrValSub(int &val, const string &cstr, int pos, int unitsec){
	return getStrValSubDelimit(val, cstr, pos, unitsec, DELIMIT_SPACE_ONLY);
}

//---------------------------------------------------------------------
// ��؂�I��ǉ����ĉ��Z���s
//---------------------------------------------------------------------
int CnvStrTime::getStrValSubDelimit(int &val, const string &cstr, int pos, int unitsec, DelimtStrType type){
	int st, ed;

	pos = getStrItemRange(st, ed, cstr, pos, type);
	try{
		val = getStrCalc(cstr, st, ed-1, unitsec);
	}
	catch(int errloc){
//		printf("err:%d\n",errloc);
		val = errloc;
		pos = -1;
	}
	return pos;
}


//---------------------------------------------------------------------
// �P���ڂ̕�����ʒu�͈͂��擾
// ���́F
//   cstr : ������
//   pos  : �ǂݍ��݊J�n�ʒu
//   type : ��ށi0=�X�y�[�X��؂�QUOTE��  1=�X�y�[�X��؂�  2=1+�R���}����؂�  3=�ŏ��̐��������̂�
// �o�́F
//   �Ԃ�l�F �ǂݍ��ݏI���ʒu
//   st   : �F���J�n�ʒu
//   ed   : �F���I���ʒu
//---------------------------------------------------------------------
int CnvStrTime::getStrItemRange(int &st, int &ed, const string &cstr, int pos, DelimtStrType type){
	if (pos < 0) return pos;

	char ch;
	int flag_quote = 0;
	int len = 0;

	//--- trim left ---
	while( isCharTypeSpace(cstr[pos]) ){
		pos ++;
	}
	//--- check quote ---
	if (cstr[pos] == '\"' && type == DELIMIT_SPACE_QUOTE){
		flag_quote ++;
		pos ++;
	}
	//--- �J�n�ʒu�ݒ� ---
	st = pos;
	//--- �f�[�^�ʒu�m�F ---
	int flag_end = 0;
	do{
		ch = cstr[pos];
		if (ch == '\"' && flag_quote > 0){				// ���p���Q���
			flag_quote ++;
			flag_end = 1;
		}
		else if (isCharTypeSpace(ch) ||				// �X�y�[�X
			(ch == ',' && type == DELIMIT_SPACE_COMMA) ||					// �R���}
			((ch < '0' || ch > '9') && type == DELIMIT_SPACE_EXNUM)){		// �����ȊO
			if (type != DELIMIT_SPACE_QUOTE || flag_quote != 1){
				flag_end = 1;				// type=DELIMIT_SPACE_QUOTE�ł͈��p���m�F
			}
		}
		else if (ch == '\0' || len >= SIZE_BUF_MAX-1){	// �����I������
			flag_end = 1;
		}
		if (flag_end == 0){
			pos ++;
			len ++;
		}
	} while(flag_end == 0);
	//--- �I���ʒu�ݒ� ---
	ed = pos;
	if (flag_quote == 1) {					// QUOTE�ُ�
		pos = -1;
	}
	else if (st == ed && flag_quote <= 1){	// �ǂݍ��݃f�[�^�Ȃ��ꍇ
		pos = -1;
	}
	else if (ch == ',' || ch == '\"'){		// ��؂蕶���͎���X�L�b�v
		pos ++;
	}
	return pos;
}

//---------------------------------------------------------------------
// �����̎�ނ��擾
//---------------------------------------------------------------------
CnvStrTime::CharCtrType CnvStrTime::getCharTypeSub(char ch){
	CharCtrType typeMark;

	switch(ch){
		case '\0':
			typeMark = CHAR_CTR_NULL;
			break;
		case ' ':
		case '\t':
			typeMark = CHAR_CTR_SPACE;
			break;
		default:
			if (ch >= 0 && ch < ' '){
				typeMark = CHAR_CTR_CTRL;
			}
			else{
				typeMark = CHAR_CTR_OTHER;
			}
			break;
	}
	return typeMark;
}


//---------------------------------------------------------------------
// �������󔒂��`�F�b�N
//---------------------------------------------------------------------
bool CnvStrTime::isCharTypeSpace(char ch){
	CharCtrType typeMark;

	typeMark = getCharTypeSub(ch);
	if (typeMark == CHAR_CTR_SPACE){
		return true;
	}
	return false;
}



//---------------------------------------------------------------------
// ����������Z�������ă~�����Ԃ��擾
// ���́F
//   cstr : ������
//   st   : �F���J�n�ʒu
//   ed   : �F���I���ʒu
//   unitsec : ���������̒P�ʁi0=�t���[����  1=�b��  2=�ϊ��Ȃ��j
// �o�́F
//   �Ԃ�l�F ���Z���ʃ~���b
//---------------------------------------------------------------------
int CnvStrTime::getStrCalc(const string &cstr, int st, int ed, int unitsec){
	return getStrCalcDecode(cstr, st, ed, unitsec, 0);
}

//---------------------------------------------------------------------
// ����������Z�������ă~�����Ԃ��擾�͈͎̔w�艉�Z
// ���́F
//   cstr : ������
//   st   : �F���J�n�ʒu
//   ed   : �F���I���ʒu
//   dsec : �������̒l�i0=�t���[����  1=�b���j
//   draw : �揜�Z����̏����i0=�ʏ�  1=�P�ʕϊ����~  2=�ϊ��Ȃ��j
// �o�́F
//   �Ԃ�l�F ���Z���ʃ~���b
//---------------------------------------------------------------------
int CnvStrTime::getStrCalcDecode(const string &cstr, int st, int ed, int dsec, int draw){
	//--- ���ɉ��Z���s���Q���ɕ������� ---
	int codeMark_op  = 0;				// ���Z�q�^�C�v
	int priorMark_op = 0;				// ���Z�q�D�揇��
	int nPar_i    = 0;					// ���݂̊��ʐ�
	int nPar_op   = -1;					// ���Z�̂��銇�ʐ��̍ŏ��l
	int posOpS    = -1;					// �������鉉�Z�q�ʒu�i�J�n�j
	int posOpE    = -1;					// �������鉉�Z�q�ʒu�i�I���j
	int flagHead  = 1;					// �P�����Z�q�t���O
	int flagTwoOp = 0;					// 2�������Z�q
	for(int i=st; i<=ed; i++){
		if (flagTwoOp > 0){				// �O��2�������Z�q�������ꍇ�͎��̕�����
			flagTwoOp = 0;
			continue;
		}
		int codeMark_i = getStrCalcCodeChar(cstr[i], flagHead);
		if (i < ed){								// 2�������Z�q�`�F�b�N
			int codeMark_two = getStrCalcCodeTwoChar(cstr[i], cstr[i+1], flagHead);
			if (codeMark_two > 0){
				codeMark_i = codeMark_two;
				flagTwoOp = 1;
			}
		}
		int categMark_i = getMarkCategory(codeMark_i);
		int priorMark_i = getMarkPrior(codeMark_i);
		if (codeMark_i == D_CALCOP_PARS){			// ���ʊJ�n
			nPar_i ++;
		}
		else if (codeMark_i == D_CALCOP_PARE){		// ���ʏI��
			nPar_i --;
			if (nPar_i < 0){						// ���ʂ̐�������Ȃ��G���[
				throw i;
			}
		}
		else if (categMark_i == D_CALCCAT_OP1){		// �P�����Z�q
		}
		else if (categMark_i == D_CALCCAT_OP2){		// 2�����Z�q
			if ((nPar_op == nPar_i && priorMark_op <= priorMark_i) ||
				(nPar_op > nPar_i) || posOpS < 0){
				posOpS = i;							// �ʒu
				posOpE = (flagTwoOp > 0)? i+1 : i;	// �ʒu
				priorMark_op = priorMark_i;			// �D�揇��
				codeMark_op  = codeMark_i;			// 2�����Z�q�f�[�^
				nPar_op      = nPar_i;				// ���ʐ�
			}
			flagHead = 1;							// ���Ɍ���镶���͒P�����Z�q
		}
		else{										// ���l����
			flagHead = 0;							// �P�����Z�q�t���O�͏���
			if (posOpS < 0 && (nPar_op > nPar_i || nPar_op < 0)){
				nPar_op = nPar_i;					// ���Z�q�Ȃ��ꍇ�̊��ʐ��ێ�
			}
		}
	}
	if (nPar_i != 0){								// ���ʂ̐�������Ȃ��G���[
		throw ed;
	}
	//--- �s�v�ȊO���̊��ʂ͊O�� ---
	int flagLoop = 1;
	while(nPar_op > 0 && flagLoop > 0){				// ���ʊO���Z���Ȃ��ꍇ���Ώ�
		int codeMark_s = getStrCalcCodeChar(cstr[st], 1);
		int codeMark_e = getStrCalcCodeChar(cstr[ed], 0);
		if (codeMark_s == D_CALCOP_PARS && codeMark_e == D_CALCOP_PARE){
			st ++;
			ed --;
			nPar_op --;
		}
		else{										// �O�������ʈȊO�Ȃ�I��
			flagLoop = 0;
		}
	}
	//--- ���Z�̎��s ---
	int dr;
	if (posOpS > 0 && nPar_op == 0){				// ���̏�����2�����Z�q�̏ꍇ
		if (posOpS == st || posOpE == ed){			// �O��ɍ��ڂ��Ȃ��ꍇ�̓G���[
			throw posOpS;
		}
		int raw2 = draw;
		if (codeMark_op == D_CALCOP_MUL ||			// �揜�Z�ł͂Q���ڂ̒P�ʕϊ����Ȃ�
			codeMark_op == D_CALCOP_DIV){
			raw2 = 1;
		}
		int d1 = getStrCalcDecode(cstr, st, posOpS-1, dsec, draw);	// �͈͑I�����čăf�R�[�h
		int d2 = getStrCalcDecode(cstr, posOpE+1, ed, dsec, raw2);	// �͈͑I�����čăf�R�[�h
		dr = getStrCalcOp2(d1, d2, codeMark_op);					// 2�����Z����
	}
	else{											// ���̏�����2�����Z�q�łȂ��ꍇ
		int codeMark_s = getStrCalcCodeChar(cstr[st], 1);
		int categMark_s = getMarkCategory(codeMark_s);
		if (categMark_s == D_CALCCAT_OP1){						// ���̏������P�����Z�q�̏ꍇ
			if (codeMark_s == D_CALCOP_SEC){
				dsec = 1;
			}
			else if (codeMark_s == D_CALCOP_FRM){
				dsec = 0;
			}
			int d1 = getStrCalcDecode(cstr, st+1, ed, dsec, draw);	// �͈͑I�����čăf�R�[�h
			dr = getStrCalcOp1(d1, codeMark_s);					// �P�����Z����
		}
		else{
			dr = getStrCalcTime(cstr, st, ed, dsec, draw);			// ���l���Ԃ̎擾
//printf("[%c,%d,%d,%d]",cstr[st],dr,st,ed);
		}
	}

	return dr;
}


//---------------------------------------------------------------------
// ������ނ̎擾 - ����
//---------------------------------------------------------------------
int CnvStrTime::getMarkCategory(int code){
	return  (code / 0x1000);
}

//---------------------------------------------------------------------
// ������ނ̎擾 - �D�揇��
//---------------------------------------------------------------------
int CnvStrTime::getMarkPrior(int code){
	return ((code % 0x1000) / 0x100);
}

//---------------------------------------------------------------------
// ���Z�p�ɕ����F��
// ���́F
//   ch   : �F�������镶��
//   head : 0=�ʏ�  1=�擪�����Ƃ��ĔF��
// �o�́F
//   �Ԃ�l�F �F���R�[�h
//---------------------------------------------------------------------
int CnvStrTime::getStrCalcCodeChar(char ch, int head){
	int codeMark;

	if (ch >= '0' && ch <= '9'){
		codeMark = ch - '0';
	}
	else{
		switch(ch){
			case '.':
				codeMark = D_CALCOP_PERD;
				break;
			case ':':
				codeMark = D_CALCOP_COLON;
				break;
			case '+':
				codeMark = D_CALCOP_PLUS;
				if (head > 0){
					codeMark = D_CALCOP_SIGNP;
				}
				break;
			case '-':
				codeMark = D_CALCOP_MINUS;
				if (head > 0){
					codeMark = D_CALCOP_SIGNM;
				}
				break;
			case '*':
				codeMark = D_CALCOP_MUL;
				break;
			case '/':
				codeMark = D_CALCOP_DIV;
				break;
			case '%':
				codeMark = D_CALCOP_MOD;
				break;
			case '!':
				codeMark = D_CALCOP_NOT;
				break;
			case 'S':
				codeMark = D_CALCOP_SEC;
				break;
			case 'F':
				codeMark = D_CALCOP_FRM;
				break;
			case '(':
				codeMark = D_CALCOP_PARS;
				break;
			case ')':
				codeMark = D_CALCOP_PARE;
				break;
			case '<':
				codeMark = D_CALCOP_CMPLT;
				break;
			case '>':
				codeMark = D_CALCOP_CMPGT;
				break;
			case '&':
				codeMark = D_CALCOP_B_AND;
				break;
			case '^':
				codeMark = D_CALCOP_B_XOR;
				break;
			case '|':
				codeMark = D_CALCOP_B_OR;
				break;
			default:
				codeMark = -1;
				break;
		}
	}
	return codeMark;
}

//---------------------------------------------------------------------
// ���Z�p�ɂQ�������Z�q�̕����F��
// ���́F
//   ch1   : �F�������镶���i�P�����ځj
//   ch2   : �F�������镶���i�Q�����ځj
//   head : 0=�ʏ�  1=�擪�����Ƃ��ĔF��
// �o�́F
//   �Ԃ�l�F �F���R�[�h
//---------------------------------------------------------------------
int CnvStrTime::getStrCalcCodeTwoChar(char ch1, char ch2, int head){
	int codeMark = -1;

	switch(ch1){
		case '=':
			if (ch2 == '='){
				codeMark = D_CALCOP_CMPEQ;
			}
			else if (ch2 == '<'){
				codeMark = D_CALCOP_CMPLE;
			}
			else if (ch2 == '>'){
				codeMark = D_CALCOP_CMPGE;
			}
			break;
		case '<':
			if (ch2 == '='){
				codeMark = D_CALCOP_CMPLE;
			}
			break;
		case '>':
			if (ch2 == '='){
				codeMark = D_CALCOP_CMPGE;
			}
			break;
		case '!':
			if (ch2 == '='){
				codeMark = D_CALCOP_CMPNE;
			}
			break;
		case '&':
			if (ch2 == '&'){
				codeMark = D_CALCOP_L_AND;
			}
			break;
		case '|':
			if (ch2 == '|'){
				codeMark = D_CALCOP_L_OR;
			}
			break;
		case '+':
			if (ch2 == '+' && head == 1){
				codeMark = D_CALCOP_P_INC;
			}
			break;
		case '-':
			if (ch2 == '-' && head == 1){
				codeMark = D_CALCOP_P_DEC;
			}
			break;
	}
	return codeMark;
}

//---------------------------------------------------------------------
// ��������~���b���Ԃɕϊ�
// ���́F
//   cstr : ������
//   st   : �F���J�n�ʒu
//   ed   : �F���I���ʒu
//   dsec : �������̒l�i0=�t���[����  1=�b��  2=�ϊ��Ȃ��j
//   draw : �揜�Z����̏����i0=�ʏ�  1=�P�ʕϊ����~�j
// �o�́F
//   �Ԃ�l�F ���Z���ʃ~���b
//---------------------------------------------------------------------
int CnvStrTime::getStrCalcTime(const string &cstr, int st, int ed, int dsec, int draw){

	//--- �����񂩂琔�l���擾 ---
	int categMark_i;
	int codeMark_i;
	int vin = 0;				// �����������Z�r��
	int val = 0;				// �����������l����
	int vms = 0;				// �~���b�������l����
	int flag_sec = 0;			// 1:���Ԃł̋L��
	int flag_prd = 0;			// �~���b�̃s���I�h�F��
	int mult_prd = 0;			// �~���b�̉��Z�P��
	for(int i=st; i<=ed; i++){
		codeMark_i = getStrCalcCodeChar(cstr[i], 0);
		categMark_i = getMarkCategory(codeMark_i);
		if (categMark_i == D_CALCCAT_IMM){			// �f�[�^
			if (codeMark_i == D_CALCOP_COLON){		// �����b�̋�؂�
				flag_sec = 1;
				val = (val + vin) * 60;
				vin = 0;
			}
			else if (codeMark_i == D_CALCOP_PERD){	// �~���b�ʒu�̋�؂�
				flag_sec = 1;
				flag_prd ++;
				mult_prd = 100;
				val += vin;
				vin = 0;
			}
			else{
				if (flag_prd == 0){				// ��������
					vin = vin * 10 + codeMark_i;
				}
				else if (flag_prd == 1){		// �~���b����
					vms = codeMark_i * mult_prd + vms;
					mult_prd = mult_prd / 10;
				}
			}
		}
		else{
			throw i;				// ���Ԃ�\�������ł͂Ȃ��G���[
		}
	}
	val += vin;
	//--- �P�ʕϊ����ďo�� ---
	int data;
	if (draw > 0 || dsec == 2){		// �P�ʕϊ����Ȃ��ꍇ
		data = val;
	}
	else if (flag_sec == 0){		// ���͕�����͐����f�[�^
		if (dsec == 0){				// �������̓t���[���P�ʂ̐ݒ莞
			data = getMsecFromFrm(val);
		}
		else{						// �������͕b�P�ʂ̐ݒ莞
			data = val * 1000;
		}
	}
	else{							// ���͕�����͎��ԃf�[�^
		data = val * 1000 + vms;
	}
	return data;
}


//---------------------------------------------------------------------
// �P�����Z
// ���́F
//   din   : ���Z���l
//   codeMark : �P�����Z�q
// �o�́F
//   �Ԃ�l�F ���Z���ʃ~���b
//---------------------------------------------------------------------
int CnvStrTime::getStrCalcOp1(int din, int codeMark){
	int ret;

	switch(codeMark){
		case D_CALCOP_NOT:
			ret = 0;
			if (din == 0){
				ret = 1;
			}
			break;
		case D_CALCOP_SIGNP:
			ret = din;
			break;
		case D_CALCOP_SIGNM:
			ret = din * -1;
			break;
		default:
			ret = din;
			break;
	}
	return ret;
}

//---------------------------------------------------------------------
// �Q�����Z
// ���́F
//   din1  : ���Z���l
//   din2  : ���Z���l
//   codeMark : �Q�����Z�q
// �o�́F
//   �Ԃ�l�F ���Z���ʃ~���b
//---------------------------------------------------------------------
int CnvStrTime::getStrCalcOp2(int din1, int din2, int codeMark){
	int ret;

//printf("(%d,%d,%d)",din1,din2,codeMark);
	switch(codeMark){
		case D_CALCOP_PLUS:
			ret = din1 + din2;
			break;
		case D_CALCOP_MINUS:
			ret = din1 - din2;
			break;
		case D_CALCOP_MUL:
			ret = din1 * din2;
			break;
		case D_CALCOP_DIV:
			ret = din1 / din2;
//			ret = (din1 + (din2/2)) / din2;
			break;
		case D_CALCOP_MOD:
			ret = din1 % din2;
			break;
		case D_CALCOP_CMPLT:
			ret = (din1 < din2)? 1 : 0;
			break;
		case D_CALCOP_CMPLE:
			ret = (din1 <= din2)? 1 : 0;
			break;
		case D_CALCOP_CMPGT:
			ret = (din1 > din2)? 1 : 0;
			break;
		case D_CALCOP_CMPGE:
			ret = (din1 >= din2)? 1 : 0;
			break;
		case D_CALCOP_CMPEQ:
			ret = (din1 == din2)? 1 : 0;
			break;
		case D_CALCOP_CMPNE:
			ret = (din1 != din2)? 1 : 0;
			break;
		case D_CALCOP_B_AND:
			ret = (din1 & din2);
			break;
		case D_CALCOP_B_XOR:
			ret = (din1 ^ din2);
			break;
		case D_CALCOP_B_OR:
			ret = (din1 | din2);
			break;
		case D_CALCOP_L_AND:
			ret = (din1 && din2);
			break;
		case D_CALCOP_L_OR:
			ret = (din1 || din2);
			break;
		default:
			ret = din1;
			break;
	}
	return ret;
}



