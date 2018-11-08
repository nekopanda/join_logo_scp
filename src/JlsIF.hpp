//
// join_logo_scp�Ăяo���N���X
//
#pragma once

class JlsDataset;
class JlsScript;

//--- �f�[�^�i�[�p ---
class JlsIF
{
private:
	//--- �V�[���`�F���W�ƍ\����؂�p�i���̓f�[�^�j ---
	struct DataScpIF {
	// �Ǎ��f�[�^
		int		msec;				// �~���b
		int		msbk;				// �I���ʒu�~���b
		int		msmute_s;			// �V�[���`�F���W�������o�J�n�ʒu
		int		msmute_e;			// �V�[���`�F���W�������o�I���ʒu
		int		still;				// �Î~��
	};
	//--- ���S�f�[�^�ێ��p�i���̓f�[�^�j ---
	struct DataLogoIF {
	// �Ǎ��f�[�^
		int		rise;				// ���S�e�J�n�t���[��
		int		fall;				// ���S�e�I���t���[��
		int		rise_l;				// ���S�e�J�n�t���[�����J�n
		int		rise_r;				// ���S�e�J�n�t���[�����I��
		int		fall_l;				// ���S�e�I���t���[�����J�n
		int		fall_r;				// ���S�e�I���t���[�����I��
		int		fade_rise;			// �e�J�n�t�F�[�h�C�����(0 or fadein)
		int		fade_fall;			// �e�I���t�F�[�h�A�E�g���(0 or fadeout)
		int		intl_rise;			// �C���^�[���[�X���(0:ALL 1:TOP 2:BTM)
		int		intl_fall;			// �C���^�[���[�X���(0:ALL 1:TOP 2:BTM)
	};

public:
	enum ErrBootOptType {
		ERROPT_NONE    = 0,
		ERROPT_EXIT    = 1,
		ERROPT_SETTING = 2
	};
	enum ErrGetOneType {
		GETONE_ERR    = -2,
		GETONE_EXIT   = -1,
		GETONE_NONE   = 0
	};

public:
	JlsIF();
	virtual ~JlsIF();
	void setArgFull(int argc, char *argv[]);		// �����ݒ�i�܂Ƃ߂āj
	void setArgEach(char *str);						// �����ݒ�i�P���j
	int  runScript();								// JL�X�N���v�g���s

private:
	int  expandArg(JlsScript &funcScript, std::vector <std::string> &listin);
	int  expandArgFromFile(JlsScript &funcScript, const std::string &fname);
	int  expandArgOne(JlsScript &funcScript, int argrest, const char* strv, const char* str1, const char* str2);
	int  readLogoframe(const std::string &fname);
	int  readScpos(const std::string &fname);
	void outputResultTrim(const std::string &outfile);
	void outputResultDetail(const std::string &outscpfile);
	void outputResultDiv(const std::string &outdivfile);
	// �f�[�^����
	void pushRecordLogo(DataLogoIF &dt);
	void pushRecordScp(DataScpIF &dt);
	void popRecordLogo();
	int  sizeDataScp();
	bool emptyDataLogo();
	void getRecordScp(DataScpIF &dt, int n);
	void setRecordScp(DataScpIF &dt, int n);
	void clearRecord(DataLogoIF &dt);
	void clearRecord(DataScpIF &dt);

private:
	//--- �֐��|�C���^ ---
	std::unique_ptr <JlsDataset>	m_funcDataset;		// �f�[�^�{�̊i�[
	JlsDataset						*pdata;				// �f�[�^�|�C���^

	//--- �I�v�V�����ێ� ---
	std::vector <std::string> m_listarg;

	//--- �t�@�C�����ێ� ---
	std::string m_logofile;			// ���S�t�@�C����
	std::string m_scpfile;			// �����V�[���`�F���W�t�@�C����
	std::string m_cmdfile;			// JL�X�N���v�g�t�@�C����
	std::string m_outfile;			// �o�͌��ʃt�@�C����
	std::string m_outscpfile;		// �ڍ׏��o�̓t�@�C����
	std::string m_outdivfile;		// �������o�̓t�@�C����
};

