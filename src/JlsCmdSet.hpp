//
// JL�X�N���v�g�p�R�}���h���e�i�[�f�[�^
//
#pragma once

///////////////////////////////////////////////////////////////////////
//
// JL�X�N���v�g�R�}���h�ݒ�l
//
///////////////////////////////////////////////////////////////////////
class JlsCmdArg
{
private:
	struct CmdArgTack {						// �ݒ���e�g�ݍ��킹���猈�肳�����s�p�ݒ�
		bool	floatBase;					// 0:���S�ʒu�  1:���ʈʒu�
		bool	virtualLogo;				// 0:���ۂ̃��S  1:�������S�������S
		bool	ignoreComp;					// 0:�ʏ�  1:���S�m���Ԃł����s
		bool	limitByLogo;				// 0:�ʏ�  1:�אڃ��S�܂łɐ���
		bool	onePoint;					// 0:�ʏ�  1:���S�P�ӏ��ɐ���
		bool	needAuto;					// 0:�ʏ�  1:Auto�\���K�v
	};
	struct CmdArgCond {						// ��͎��̏��
		int		posStr;						// �V�X�e���R�}���h�p������ʒu
		bool	flagCond;					// IF���p�̏������f
	};
	struct CmdArgSc {					// -SC�n�̃I�v�V�����f�[�^
		int		type;
		Msec	min;
		Msec	max;
	};

public:
	JlsCmdArg();
	void	clear();
// ��ʃI�v�V�����p
	void	setOpt(int dselect, int val);
	bool	isSetOpt(int dselect);
	int		getOpt(int dselect);
// -SC�n�R�}���h�p
	void	addScOpt(int numdata, int min, int max);
	JlOptionArgScType	getScOptType(int num);
	bool	isScOptRelative(int num);
	Msec	getScOptMin(int num);
	Msec	getScOptMax(int num);
	int		sizeScOpt();
// -LG�n�R�}���h�p
	void	addLgOpt(int nlg);
	int		getLgOpt(int num);
	int		sizeLgOpt();

public:
// �R�}���h
	JlcmdSelType		cmdsel;				// �R�}���h�I��
	JlcmdCategoryType	category;			// ���s���̃R�}���h���
	WideMsec			wmsecDst;			// �ΏۑI��͈�
	LogoEdgeType		selectEdge;			// S/E/B
	JlcmdSubType		selectAutoSub;		// TR/SP/EC
// �������
	CmdArgTack			tack;				// �ݒ���e�g�ݍ��킹���猈�肳�����s�p�ݒ�
	CmdArgCond			cond;				// ��͎��̏��

private:
// ��ʃI�v�V�����ۑ�
	int					optdata[SIZE_JLOPT_DATA];
	int					flagset[SIZE_JLOPT_DATA];
// ���X�g�ۑ�
	vector<CmdArgSc>	listScOpt;	// -SC�n�I�v�V�����ێ�
	vector<int>			listLgVal;	// ���S�ԍ����ۑ�
	vector<Msec>		listTLOpt;	// -TL�I�v�V�����ێ�
};



///////////////////////////////////////////////////////////////////////
//
// JL�X�N���v�g�R�}���h�ݒ蔽�f�p
//
///////////////////////////////////////////////////////////////////////
class JlsCmdLimit
{
private:
	enum CmdProcessFlag {					// �ݒ��ԋL���p
		ARG_PROCESS_HEADTAIL    = 0x01,
		ARG_PROCESS_FRAMELIMIT  = 0x02,
		ARG_PROCESS_VALIDLOGO   = 0x04,
		ARG_PROCESS_BASELOGO    = 0x08,
		ARG_PROCESS_TARGETRANGE = 0x10,
		ARG_PROCESS_SCPENABLE   = 0x20,
		ARG_PROCESS_RESULT      = 0x40,
	};
	struct ArgValidLogo {					// �L�����S���X�g�擾�p
		Msec			msec;
		LogoEdgeType	edge;
	};

public:
	JlsCmdLimit();
	void			clear();
	RangeMsec		getHeadTail();
	Msec			getHead();
	Msec			getTail();
	bool			setHeadTail(RangeMsec rmsec);
	bool			setFrameRange(RangeMsec rmsec);
	RangeMsec		getFrameRange();
	Msec			getLogoListMsec(int nlist);
	LogoEdgeType	getLogoListEdge(int nlist);
	bool			addLogoList(Msec &rmsec, jlsd::LogoEdgeType edge);
	int				sizeLogoList();
	Nrf				getLogoBaseNrf();
	Nsc				getLogoBaseNsc();
	LogoEdgeType	getLogoBaseEdge();
	bool			setLogoBaseNrf(Nrf nrf, jlsd::LogoEdgeType edge);
	bool			setLogoBaseNsc(Nsc nsc, jlsd::LogoEdgeType edge);
	WideMsec		getTargetRangeWide();
	Msec			getTargetRangeForce();
	bool			isTargetRangeLogo();
	bool			setTargetRange(WideMsec wmsec, Msec msec_force, bool from_logo);
	bool			isTargetListed(Msec msec_target);
	void			clearTargetList();
	void			addTargetList(RangeMsec rmsec);
	bool			getScpEnable(Nsc nsc);
	bool			setScpEnable(vector<bool> &listEnable);
	int 			sizeScpEnable();
	Nsc				getResultTargetSel();
	Nsc				getResultTargetEnd();
	bool			setResultTarget(Nsc nscSelIn, Nsc nscEndIn);

private:
	void			signalInternalError(CmdProcessFlag flags);

private:
	RangeMsec		rmsecHeadTail;			// $HEADTIME/$TAILTIME����
	RangeMsec		rmsecFrameLimit;		// -F�I�v�V��������
	vector<ArgValidLogo>	listValidLogo;	// �L�����S�ʒu�ꗗ
	Nrf				nrfBase;				// ��ʒu�̎����S�ԍ�
	Nsc				nscBase;				// ��ʒu�̐����\�����S���������V�[���`�F���W�ԍ�
	LogoEdgeType	edgeBase;				// ��ʒu�̃G�b�W�I��
	WideMsec		wmsecTarget;			// �Ώۈʒu�͈�
	Msec			msecTargetFc;			// �����ݒ�p�Ώۈʒu
	bool			fromLogo;				// ���S��񂩂�̑Ώۈʒu�͈�
	vector<RangeMsec>	listTLRange;		// �Ώۈʒu�Ƃ��ċ�����͈̓��X�g
	vector<bool>	listScpEnable;			// �����V�[���`�F���W�I��
	Nsc				nscSel;					// �Ώۈʒu�����V�[���`�F���W�ԍ�
	Nsc				nscEnd;					// -endlen�ɑΉ����閳���V�[���`�F���W�ԍ�

	int				process;				// �ݒ��ԕێ�
};



///////////////////////////////////////////////////////////////////////
//
// JL�X�N���v�g�R�}���h�S��
//
///////////////////////////////////////////////////////////////////////
class JlsCmdSet
{
public:
	JlsCmdArg		arg;			// �ݒ�l
	JlsCmdLimit		limit;			// �ݒ蔽�f
};
