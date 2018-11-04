//
// join_logo_scp �f�[�^�\�������␳
//
#include "stdafx.h"
#include "CommonJls.hpp"
#include "JlsReformData.hpp"
#include "JlsDataset.hpp"


//---------------------------------------------------------------------
// �\�z
//---------------------------------------------------------------------
JlsReformData::JlsReformData(JlsDataset *pdata){
	//--- �֐��|�C���^�쐬 ---
	this->pdata = pdata;
}

//---------------------------------------------------------------------
// ���̓f�[�^����̔��������s������
//---------------------------------------------------------------------
void JlsReformData::adjustData(){
	pdata->setFlagSetupAdj( true );
	adjustScpos();
	detectCutmrg();
	updateCutmrg();
	adjustLogo();
}



//---------------------------------------------------------------------
// CutMrgIn/CutMrgOut ��ݒ�
//---------------------------------------------------------------------
void JlsReformData::updateCutmrg(){
	int num_logo  = pdata->sizeDataLogo();

	for(int i=0; i<num_logo; i++){
		DataLogoRecord dtlogo;
		pdata->getRecordLogo(dtlogo, i);

		// rise�ݒ�
		{
			int intl = dtlogo.intl_rise;
			int msec_cutin = pdata->extOpt.msecCutIn;
			if (intl > 0){
				Msec msec_org = dtlogo.org_rise;
				Msec msec_nxt = pdata->cnv.getMsecAdjustFrmFromMsec(msec_org, 1);
				msec_cutin += (msec_nxt - msec_org) / 2;
			}
			int n1 = dtlogo.org_rise   - msec_cutin;
			int n2 = dtlogo.org_rise_l - msec_cutin;
			int n3 = dtlogo.org_rise_r - msec_cutin;
			if (n1 < 0){
				n1 = 0;
			}
			if (n2 < 0){
				n2 = 0;
			}
			if (n3 < 0){
				n3 = 0;
			}
			//--- �V�[���`�F���W�ʒu�ɂ��X�V���f ---
			int nsc_base = pdata->getNscFromMsecAll(dtlogo.org_rise);
			int msec_base = pdata->getMsecScp(nsc_base);
			if (nsc_base > 0){
				if (pdata->getScpStill(nsc_base)){		// �����Ȃ��ʒu�͏���
					nsc_base = -1;
				}
			}
			if (pdata->extOpt.wideCutIn == 0 ||
				(pdata->extOpt.wideCutIn == 2 && msec_cutin > pdata->msecValNear3) ||
				nsc_base < 0 ||
				dtlogo.org_rise_l - pdata->msecValExact > msec_base ||
				dtlogo.org_rise_r + pdata->msecValExact < msec_base ||
				(dtlogo.org_rise - msec_base >= (dtlogo.org_rise - n1) / 2)){
				//--- �X�V ---
				dtlogo.rise   = pdata->cnv.getMsecAlignFromMsec( n1 );
				dtlogo.rise_l = pdata->cnv.getMsecAlignFromMsec( n2 );
				dtlogo.rise_r = pdata->cnv.getMsecAlignFromMsec( n3 );
			}
			else{
				dtlogo.rise   = dtlogo.org_rise;
				dtlogo.rise_l = dtlogo.org_rise_l;
				dtlogo.rise_r = dtlogo.org_rise_r;
			}
		}

		// fall�ݒ�
		{
			int intl = dtlogo.intl_fall;
			int msec_cutout = pdata->extOpt.msecCutOut;
			if (intl > 0){
				Msec msec_org = dtlogo.org_fall;
				Msec msec_nxt = pdata->cnv.getMsecAdjustFrmFromMsec(msec_org, -1);
				msec_cutout += (msec_nxt - msec_org) / 2;
			}
			int n1 = dtlogo.org_fall   + msec_cutout;
			int n2 = dtlogo.org_fall_l + msec_cutout;
			int n3 = dtlogo.org_fall_r + msec_cutout;
			if (n1 < 0){
				n1 = 0;
			}
			if (n2 < 0){
				n2 = 0;
			}
			if (n3 < 0){
				n3 = 0;
			}
			int msec_max = pdata->getMsecTotalMax();
			if (n1 > msec_max){
				n1 = msec_max;
			}
			if (n2 > msec_max){
				n2 = msec_max;
			}
			if (n3 > msec_max){
				n3 = msec_max;
			}
			//--- �V�[���`�F���W�ʒu�ɂ��X�V���f ---
			int nsc_base  = pdata->getNscFromMsecAll(dtlogo.org_fall);
			int msec_base = pdata->getMsecScpBk(nsc_base);
			if (nsc_base > 0){
				if (pdata->getScpStill(nsc_base)){		// �����Ȃ��ʒu�͏���
					nsc_base = -1;
				}
			}
			if (pdata->extOpt.wideCutOut == 0 ||
				(pdata->extOpt.wideCutOut == 2 && msec_cutout > pdata->msecValNear3) ||
				nsc_base < 0 ||
				dtlogo.org_fall_l - pdata->msecValExact > msec_base ||
				dtlogo.org_fall_r + pdata->msecValExact < msec_base ||
				(msec_base - dtlogo.org_fall >= (n1 - dtlogo.org_fall) / 2)){
				//--- �X�V ---
				dtlogo.fall   = pdata->cnv.getMsecAlignFromMsec( n1 );
				dtlogo.fall_l = pdata->cnv.getMsecAlignFromMsec( n2 );
				dtlogo.fall_r = pdata->cnv.getMsecAlignFromMsec( n3 );
			}
			else{
				dtlogo.fall   = dtlogo.org_fall;
				dtlogo.fall_l = dtlogo.org_fall_l;
				dtlogo.fall_r = dtlogo.org_fall_r;
			}
		}
		pdata->setRecordLogo(dtlogo, i);
	}
}


//---------------------------------------------------------------------
// �����V�[���`�F���W�̌��I��
//---------------------------------------------------------------------
void JlsReformData::adjustScpos(){
	int num_scpos = pdata->sizeDataScp();
	vector<int> nd5_bsum(num_scpos, 0);
	vector<int> nd5_sum(num_scpos, 0);
	vector<int> nd5_nxt(num_scpos, -1);
	vector<int> nd15_flag(num_scpos, 0);

	// �T�̔{���b�̃V�[���`�F���W�Ԋu�ꏊ��T��
	for(int i=0; i<num_scpos; i++){
		int msec_src = pdata->getMsecScp(i);
		int sec_last = 0;
		if (i > 0 && i < num_scpos-1){			// �擪�ƍŌ�͏���
			for(int j=1; j<num_scpos-1; j++){		// �擪�ƍŌ�͏���
				int msec_dst = pdata->getMsecScp(j);
				CalcDifInfo calc;
				if (calcDifFunc(calc, msec_src, msec_dst) > 0){
					if (calc.sgn < 0){
						if (calc.sec % 5 == 0){
							nd5_bsum[i] ++;
						}
						if (calc.sec % 15 == 0){
							nd15_flag[i] = 1;
						}
					}
					else if (calc.sgn > 0){
						if (calc.sec % 5 == 0){
							if (nd5_sum[i] == 0){
								nd5_nxt[i] = j;
							}
							if (sec_last != calc.sec){
								nd5_sum[i] ++;
								sec_last = calc.sec;
							}
						}
						if (calc.sec % 15 == 0){
							nd15_flag[i] = 1;
						}
					}
				}
			}
		}
		if (nd5_bsum[i] > 0 || nd5_sum[i] > 0){
			pdata->setScpStatpos(i, SCP_PRIOR_LV1);
		}
		else{
			pdata->setScpStatpos(i, SCP_PRIOR_NONE);
		}
	}
	// 15�̔{���̃V�[���`�F���W�Ԋu������ꏊ�Ɋ֌W���邩�`�F�b�N
	for(int i=0; i<num_scpos; i++){
		if (pdata->getScpStatpos(i) > SCP_PRIOR_NONE){
			int flag_nd15 = 0;
			// �֌W�����`�F�b�N
			int j = i;
			while(j >= 1 && j < num_scpos-1 && flag_nd15 == 0){		// �擪�ƍŌ�͏���
				if (nd15_flag[j] > 0){		// 15�̔{���������ꍇ�t���O�𗧂Ă�
					flag_nd15 = 1;
				}
				j = nd5_nxt[j];
			}
			// �֌W�����������ꍇ�̃Z�b�g
			if (flag_nd15 > 0){
				j = i;
				while(j >= 0 && j < num_scpos){
					nd15_flag[j] = 1;
					j = nd5_nxt[j];
				}
			}
		}
	}
	for(int i=0; i<num_scpos; i++){
		if (nd15_flag[i] == 0){			// 15�̔{���Ɋ֘A�Ȃ���Γ����X�e�[�^�X������
			pdata->setScpStatpos(i, SCP_PRIOR_NONE);
		}
	}
//	for(i=0; i<num_scpos; i++){
//		printf("(%ld:%d,%d,%d)", plogo->frm_scpos_s[i], nd15_flag[i], nd5_sum[i], nd5_bsum[i]);
//	}

	// �Q�T�t���[���ȓ��ɂT�̔{���b�Ԋu�V�[���`�F���W���d�Ȃ�ꍇ�A�Е��݂̂ɂ���
	{
		int ncmp_i = -1;
		int ncmp_msec = 0;
		for(int i=1; i<num_scpos-1; i++){		// �擪�ƍŌ�͏���
			int msec_src = pdata->getMsecScp(i);
			if (msec_src - ncmp_msec >= 870 && ncmp_i >= 0){
				ncmp_i = -1;
			}
			if (pdata->getScpStatpos(i) > SCP_PRIOR_NONE){
				if (ncmp_i < 0){
					ncmp_i = i;
					ncmp_msec = msec_src;
				}
				else{
					int count_src = 0;
					int count_cmp = 0;
					for(int j=1; j<num_scpos-1; j++){		// �擪�ƍŌ�ȊO�Ŕ�r
						if (pdata->getScpStatpos(j) > SCP_PRIOR_NONE){
							int msec_dst = pdata->getMsecScp(j);
							CalcDifInfo calc1;
							CalcDifInfo calc2;
							int flag1 = calcDifFunc(calc1, msec_src, msec_dst);
							int flag2 = calcDifFunc(calc2, ncmp_msec, msec_dst);
							if (flag1 > 0 && flag2 > 0 &&
								calc1.sec % 5 == 0 && calc1.sec == calc2.sec && calc1.sec <= 60){
								if (calc1.gap < calc2.gap){
									count_src += 3;
								}
								else if (calc1.gap > calc2.gap){
									count_cmp += 3;
								}
							}
							else if (flag1 > 0 && (flag2 == 0 || calc2.sec % 5 != 0) && calc1.sec % 5 == 0){
								if (calc1.sec <= 60){
									count_src ++;
									if (calc1.gap <= 3){
										count_src ++;
									}
								}
							}
							else if (flag2 > 0 && (flag1 == 0 || calc1.sec % 5 != 0) && calc2.sec % 5 == 0){
								if (calc2.sec <= 60){
									count_cmp ++;
									if (calc2.gap <= 3){
										count_cmp ++;
									}
								}
							}
						}
					}
//printf("(0:%d-%d,%d,%d)", frm_src,ncmp_frm,count_src,count_cmp);
					if (count_src <= 1 && count_cmp <= 1){
						// �֘A�t���[���������Ȃ��ꍇ�͎c��
					}
					else if (count_src > count_cmp){
//printf("(1:%d,%d,%d)", ncmp_i,count_src,count_cmp);
						for(int j=0; j<ncmp_i; j++){		// �����ւ����Q�Ƃ����ւ���
							if (nd5_nxt[j] == ncmp_i){
								nd5_nxt[j] = i;
							}
						}
						pdata->setScpStatpos(ncmp_i, SCP_PRIOR_DUPE);
						ncmp_i = i;
						ncmp_msec = msec_src;
					}
					else{
//printf("(2:%d,%d,%d)", i,count_src,count_cmp);
						for(int j=0; j<i; j++){
							if (nd5_nxt[j] == i){	// �����ւ����Q�Ƃ����ւ���
								nd5_nxt[j] = ncmp_i;
							}
						}
						pdata->setScpStatpos(i, SCP_PRIOR_DUPE);
					}
				}
			}
		}
	}

	// �擪���珇�ԂɃV�[���`�F���W�ʒu�����ǂ��Ă���
	{
		int nst = 0;
//	plogo->stat_scpos[nst] = 2;
		while( nst<num_scpos-1 ){
			// �񓯊��I���n�_������
			int ned = nst+1;
			while((pdata->getScpStatpos(ned) <= SCP_PRIOR_NONE || nd5_sum[ned] == 0) && ned < num_scpos-1){
				ned ++;
			}
			// ������ɐ������񓯊��I���n�_���Ȃ�������
			if (ned < num_scpos-1){
				int nc_nxt = nd5_nxt[ned];
				int i = ned + 1;
				while(i < nc_nxt){
					if (pdata->getScpStatpos(i) >= SCP_PRIOR_NONE && nd5_sum[i] > 0){
						if (nd5_sum[i] > nd5_sum[ned]){		// 5�̔{���Ԋu��������Δ񓯊��I���n�_��ύX
							ned = i;
							nc_nxt = nd5_nxt[ned];
						}
					}
					i ++;
				}
			}

			// �񓯊��Q�n�_�Ԃ̃V�[���`�F���W���Ȃ�������
			// �Ԋu��5�̔{���ł͂Ȃ��Ă��񓯊����[����3�`10�b�̃V�[���`�F���W�͓����
			int msec_st = pdata->getMsecScp(nst);
			int msec_ed = pdata->getMsecScp(ned);
			pdata->setScpStatpos(ned, SCP_PRIOR_DECIDE);
			int ncmp_i = -1;
			int ncmp_msec = 0;
			for(int i=nst+1; i<ned; i++){
				int msec_dst = pdata->getMsecScp(i);
				// �P�O�̌��n�_���痣�ꂽ��P�O�̌��͊m�肷��
				if (msec_dst - ncmp_msec >= 940 && ncmp_i >= 0){
					ncmp_i = -1;
				}
				// �񓯊��Q�n�_�̂ǂ��炩����R�b�ȏ㗣��Ă��邱�Ƃ��O��
				if (msec_dst - msec_st >= 2670 && msec_ed - msec_dst >= 2670){
					int dis_ncmp = 0;
					int dis_dst = 1;
					// �O������̊Ԋu�`�F�b�N�i�t���[���O�͏����j
					CalcDifInfo calc1;
					CalcDifInfo calc2;
					if (calcDifFunc(calc1, msec_st, msec_dst) > 0 && nst > 0){
						if (calc1.sec < 10){
							dis_dst = 0;
							if (ncmp_i >= 0 && nst != 0){
								if (calcDifFunc(calc2, msec_st, ncmp_msec) > 0){
									if (calc1.gap < calc2.gap){
										dis_ncmp = 1;
									}
									else{
										dis_dst = 1;
									}
								}
							}
						}
					}
					// �������̊Ԋu�`�F�b�N
					if (calcDifFunc(calc1, msec_dst, msec_ed) > 0){
						if (calc1.sec < 10){
							dis_dst = 0;
							if (ncmp_i >= 0 && ned < num_scpos-1){
								if (calcDifFunc(calc2, ncmp_msec, msec_ed) > 0){
									if (calc1.gap < calc2.gap){
										dis_ncmp = 1;
									}
									else{
										dis_dst = 1;
									}
								}
							}
						}
					}
					if (dis_ncmp > 0){			// �P�O�̌�₪�����Ɣ��肳�ꂽ�ꍇ
						pdata->setScpStatpos(ncmp_i, SCP_PRIOR_DUPE);
						ncmp_i = -1;
					}
					if (dis_dst == 0){			// ���݂̌�₪�����ł͂Ȃ��Ɣ��肳�ꂽ�ꍇ
						pdata->setScpStatpos(i, SCP_PRIOR_DECIDE);
						ncmp_i = i;
						ncmp_msec = msec_dst;
					}
				}
			}
			// �������Ă���V�[���`�F���W�̍ŏI�ʒu���擾
			int nc_st = ned;
			int nc_nxt = nd5_nxt[nc_st];
			int i = nc_st + 1;
			while(nc_nxt > 0 && i <= nc_nxt && i <= num_scpos-1){
				if (i == nc_nxt){				// �����t���[��������
					nc_st = i;
					nc_nxt = nd5_nxt[nc_st];
					pdata->setScpStatpos(nc_st, SCP_PRIOR_DECIDE);
				}
				else if (i == num_scpos-1){		// �ŏI�t���[����
					nc_st = i;
					nc_nxt = 0;
				}
				else{
					if (nd5_sum[i] > nd5_sum[nc_st] && pdata->getScpStatpos(i) >= SCP_PRIOR_NONE){	// �����t���[�������O�Ɍ�₪�������ꍇ
						int ntmp_nxt = nd5_nxt[i];
						int msec_tmp_st = pdata->getMsecScp(i);				// �V�K���ʒu
						int msec_tmp_ed = pdata->getMsecScp(ntmp_nxt);
						int msec_tmp_st2 = pdata->getMsecScp(nc_st);		// ���̈ʒu
						int msec_tmp_ed2 = pdata->getMsecScp(nc_nxt);
						CalcDifInfo calc1;
						CalcDifInfo calc2;
						int flag1 = calcDifFunc(calc1, msec_tmp_st, msec_tmp_ed);
						int flag2 = calcDifFunc(calc2, msec_tmp_st2, msec_tmp_ed2);
						if (flag1 > 0 && calc1.gap <= pdata->msecValNear1){	// �덷�������������ꍇ�̂ݔ񓯊��n�_�Ƃ���
							if (flag2 == 0 ||
								(calc2.sec % 15 != 0 && (calc1.sec <= 30 || calc1.sec <= calc2.sec)) || 
								(msec_tmp_ed <= msec_tmp_ed2 + 10*1000)){
								nc_nxt = 0;
							}
						}
					}
				}
				i ++;
			}
			nst = nc_st;
		}
	}

	// �J�n����̃V�[���`�F���W�̃`�F�b�N
	{
//		int ncmp_i = -1;
		ScpPriorType ncmp_stat = SCP_PRIOR_DUPE;
		int i = 1;
		while(pdata->getMsecScp(i) <= 30400 && i < num_scpos){		// 30�b�ȓ����`�F�b�N
			ScpPriorType ntmp_stat = pdata->getScpStatpos(i);
			if (ntmp_stat == SCP_PRIOR_DECIDE && ncmp_stat <= SCP_PRIOR_LV1){	// �ŏ��̊m����
				ntmp_stat = SCP_PRIOR_LV1;
				if (nd15_flag[i] > 0){		// 15�̔{������ő��݂���ꍇ�̂�
					int flag1 = 0;
					int flag2 = 0;
					int j = 0;					// �O�̂��߃��[�v�h�~
					// �P��ڂ̊Ԋu�擾
					int nc_st  = i;
					int nc_nxt = nc_st;
					CalcDifInfo calc1;
					CalcDifInfo calc2;
					while(nc_nxt > 0 && flag1 <= 1 && j < 100){
						nc_nxt = nd5_nxt[nc_nxt];
						j ++;
						if (nc_nxt > 0){
							int msec_src = pdata->getMsecScp(nc_st);
							int msec_dst = pdata->getMsecScp(nc_nxt);
							flag1 = calcDifExact(calc1, msec_src, msec_dst);
						}
					}
					// �Q��ڂ̊Ԋu�擾
					nc_st  = nc_nxt;
					while(nc_nxt > 0 && flag2 <= 0 && j < 100){
						nc_nxt = nd5_nxt[nc_nxt];
						j ++;
						if (nc_nxt > 0){
							int msec_src = pdata->getMsecScp(nc_st);
							int msec_dst = pdata->getMsecScp(nc_nxt);
							flag2 = calcDifExact(calc2, msec_src, msec_dst);
						}
					}
					if (flag1 > 0 && flag2 > 0){
						ntmp_stat = SCP_PRIOR_DECIDE;
					}
				}
				pdata->setScpStatpos(i, ntmp_stat);
			}
			if (ntmp_stat > SCP_PRIOR_LV1){							// ���������ɂ���ꍇ�͉������Ȃ�
//				ncmp_i = -1;
				ncmp_stat = ntmp_stat;
			}
			else if (pdata->getMsecScp(i) <= 30400){		// �J�n30�b�ȓ������
				if (ntmp_stat > ncmp_stat){				// �ŏ��̓����Ȃ��V�[���`�F���W�����
					ncmp_stat = ntmp_stat;
//					ncmp_i = i;
				}
			}
			i ++;
		}
//		if (ncmp_i > 0){				// �ŏ��̓����Ȃ��V�[���`�F���W���������ꍇ
//			pdata->m_nchk_scpos_1st = ncmp_i;
//		}
//		else{
//			pdata->m_nchk_scpos_1st = 0;
//		}
	}

//	for(i=0; i<num_scpos; i++){
//		printf("(%ld:%d, %d,%d,%d)", plogo->frm_scpos_s[i], plogo->stat_scpos[i], nd15_flag[i], nd5_sum[i], nd5_bsum[i]);
//	}

	// �P�Ȃ���͏�����Ԃɖ߂�
	for(int i=1; i<num_scpos-1; i++){		// �擪�ƍŌ�͏���
		if (pdata->getScpStatpos(i) == SCP_PRIOR_LV1){
			pdata->setScpStatpos(i, SCP_PRIOR_NONE);
		}
	}

//	// �z�u��񏉊���
//	for(i=0; i<plogo->num_scpos; i++){
//		plogo->arstat_sc_e[i] = D_SCINT_UNKNOWN;
//	}
}


//---------------------------------------------------------------------
// CutMrgIn/CutMrgOut�ʒu�̎������o
//---------------------------------------------------------------------
void JlsReformData::detectCutmrg(){
	//--- �������o���� ---
	Msec cutin  = detectCutmrgSub(1);
	Msec cutout = detectCutmrgSub(0);
	//--- ���ݒ肾�����ꍇ�͍X�V ---
	if (pdata->extOpt.fixCutIn == 0){
		pdata->extOpt.msecCutIn = cutin;
	}
	if (pdata->extOpt.fixCutOut == 0){
		pdata->extOpt.msecCutOut = cutout;
	}

	//--- display auto detect CurMrgIn/Out ---
	if (1){
		string str_cutin;
		string str_cutout;
		// ���o�}�[�W��
		detectCutmrgDispval(str_cutin,  cutin );
		detectCutmrgDispval(str_cutout, cutout);
		printf("auto detect CutMrgIn=%s CutMrgOut=%s\n", str_cutin.c_str(), str_cutout.c_str());

		// �ݒ�}�[�W��
		detectCutmrgDispval(str_cutin,  pdata->extOpt.msecCutIn );
		detectCutmrgDispval(str_cutout, pdata->extOpt.msecCutOut);
		printf("current set CutMrgIn=%s CutMrgOut=%s\n", str_cutin.c_str(), str_cutout.c_str());
	}
}

//--- �\���p�̃}�[�W���l���擾 ---
void JlsReformData::detectCutmrgDispval(string &strdisp, Msec mrg){
	//--- 16�{�����l�̃x�[�X�l���擾 ---
	int mrg_abs = abs(mrg);
	int valbase = getFrm30fpsFromMsec(mrg_abs * 16 + 8);
	//--- ���� ---
	int valint = valbase / 16;
	int valmod = valbase % 16;
	//--- �\��������쐬 ---
	strdisp = "";
	if (mrg < 0 && (valint != 0 || valmod != 0)){
		strdisp += "-";
	}
	strdisp += to_string(valint);
	if (valmod > 0){
		int valfrac = (valmod * 100 + 8) / 16;
		strdisp += ".";
		int valf1 = valfrac / 10;
		int valf2 = valfrac % 10;
		strdisp += to_string(valf1);
		if (valf2 > 0){
			strdisp += to_string(valf2);
		}
	}
}

//---------------------------------------------------------------------
// CutMrgIn/CutMrgOut�ʒu�̎������o
// ����
//   rise : 0=CutMrgOut  1=CutMrgIn
// �o��
// �Ԃ�l�F �~���b
//---------------------------------------------------------------------
Msec JlsReformData::detectCutmrgSub(int rise){
	int num_logo  = pdata->sizeDataLogo();
	int num_scpos = pdata->sizeDataScp();

	short hist[200];
	short hist_h[200];
	int fade_cnt;
	int loc_max;
	const int MINUS = 10;

	int loop = 2;
	while(loop > 0){
		// �g�p����m��x
		ScpPriorType stat_level = SCP_PRIOR_NONE;
		if (loop == 2){
			stat_level = SCP_PRIOR_DECIDE;
		}

		fade_cnt = 0;
		int sum_scale = 0;
		for(int i=0; i<200; i++){
			hist[i] = 0;
			hist_h[i] = 0;
		}
		for(int i=0; i<num_logo; i++){
			Msec msec_logo;
			int scale, intl;
			{	// msec_logo, scale, intl, fade_cnt
				Msec msec_dif;
				int fade;
				DataLogoRecord dtlogo;
				pdata->getRecordLogo(dtlogo, i);
				if (rise > 0){
					msec_logo = dtlogo.rise;
					msec_dif  = dtlogo.rise_r - dtlogo.rise_l;
					fade      = dtlogo.fade_rise;
					intl      = dtlogo.intl_rise;
				}
				else{
					msec_logo = dtlogo.fall;
					msec_dif  = dtlogo.fall_r - dtlogo.fall_l;
					fade      = dtlogo.fade_fall * -1;
					intl      = dtlogo.intl_fall;
				}
				Msec msec_fade = 0;
				if (fade != 0){
					Msec msec_tmp = pdata->cnv.getMsecAdjustFrmFromMsec(msec_logo, fade);
					msec_fade = abs(msec_tmp - msec_logo);
				}
				// ���S�̕s�m��x�ŏd�݂Â�
				scale = 1;
				if (msec_logo <= 50 || msec_logo >= pdata->getMsecTotalMax() - 50){
					scale = 0;
				}
				else if (msec_dif <= 50 || msec_dif <= msec_fade + 50){
					scale = 3;
				}
				sum_scale += scale;
				// �t�F�[�h�ݒ肠��̔���
				if (fade != 0){
					fade_cnt += scale;
				}
				else{
					fade_cnt -= scale;
				}
			}
			// �m�薳���V�[���`�F���W���}�[�W���ʒu���Ƃ��Ēǉ����Ă���
			if (scale > 0){
				for(int j=1; j < num_scpos - 1; j++){
					Msec msec_dif;
					Msec msec_scpj = pdata->getMsecScp(j);
					if (rise > 0){
						msec_dif = msec_logo - msec_scpj;
					}
					else{
						msec_dif = msec_scpj - msec_logo;
					}
					int tmp_dif = getFrm30fpsFromMsec( msec_dif );		// 30fps���Z�Ő���
					tmp_dif += MINUS;
//printf("[%d:%d,%d]", j, tmp_dif, frm_logo);
					if (pdata->getScpStatpos(j) >= stat_level){
						if (tmp_dif >= 0 && tmp_dif < 200){
//printf("[%d,%d]", tmp_dif, frm_logo);
							hist[tmp_dif] += scale;
							if (intl > 0){
								hist_h[tmp_dif] += scale;
							}
						}
					}
				}
			}
		}

		// ��₩��ʒu�����߂�
		loc_max = 0;
		int val_max = 0;
		for(int i=2; i<200-2; i++){
			int val;
			if (fade_cnt > 0){
				val = hist[i-2] + hist[i-1] + hist[i] + hist[i+1] + hist[i+2];
			}
			else{
				val = hist[i-1] + hist[i] + hist[i+1];
			}
			if (i >= MINUS-2 && i <= 36+MINUS){	// �W���I�ȃ}�[�W���͈�
				val *= 2;						// �傫�ڂɐݒ�
			}
			else if (val == sum_scale){			// �S�ʒu�Ō��o�����ꍇ
				val = val * 5 / 3;				// �����傫�ڂɐݒ�
			}
			else if (i >= 60+MINUS){			// 2�b�ȏ�͂قڑΏۊO
				val /= 4;
			}
			if (val_max < val){
				val_max = val;
				loc_max = i;
			}
			else if (val_max == val && loc_max > 0){
				if (hist_h[loc_max-1] < (hist[i+1] - hist_h[i+1])){
					val_max = val;
					loc_max = i;
				}
			}
		}
		// ��₪���݂��邩�m�F
		if (num_logo == 1 && val_max > 0){		// ���S�P�����̏ꍇ
			loop = 0;
		}
		if (val_max <= 1 || val_max <= num_logo * 2/3){	// ���s��
			loop --;
			loc_max = 0;
		}
		else{
			loop = 0;
		}
	}

	// ���̒�����m�肳����
	int loc_det = MINUS;
	int loc_half = 0;
	if (loc_max >= 2){
		if (fade_cnt <= 0){				// �t�F�[�h�Ȃ�
			loc_det = loc_max-1;
			int val_max = 0;
			short tmp_hist1[3];
			short tmp_hist2[3];
			for(int i=loc_max-1; i<=loc_max+1; i++){
				int val = hist[i];
				tmp_hist1[i-loc_max+1] = hist[i] - hist_h[i];
				tmp_hist2[i-loc_max+1] = hist_h[i];
				if (val_max < val){
					val_max = val;
					loc_det = i;
				}
			}
//printf("(%d,%d:%d,%d,%d - %d,%d,%d)", loc_det,loc_max,
// tmp_hist1[0],tmp_hist1[1],tmp_hist1[2],tmp_hist2[0],tmp_hist2[1],tmp_hist2[2]);
			if (loc_det == loc_max-1){
				if (tmp_hist2[0] < 3){		// HALF�f�[�^�Ȃ�
					loc_half = 0;
					if (loc_det <= MINUS){
						loc_half = 1;
					}
				}
				else if ((tmp_hist1[1] + tmp_hist1[2])*5 <= tmp_hist1[0]){ // �S���ŏ�
					loc_half = 1;
				}
				else{				// ALL:���� HALF:�ŏ�
					loc_det += 1;
					loc_half = 0;
				}
			}
			else if (loc_det == loc_max + 1){
				if (tmp_hist2[0] < 3 && tmp_hist2[1] < 3 && tmp_hist2[2] < 3){	// HALF�f�[�^�Ȃ�
					loc_half = 0;
					if (tmp_hist1[1] >= 3 && tmp_hist1[1] > tmp_hist1[0] * 2){
						loc_det -= 1;
					}
					if (loc_det <= MINUS){
						loc_half = 1;
					}
				}
				else if (tmp_hist2[0] >= 3 && tmp_hist2[0] > tmp_hist2[2] * 2){
					loc_det -= 1;		// ALL:���� HALF:�ŏ�
					loc_half = 0;
				}
				else if (tmp_hist2[1] >= 3 && tmp_hist2[1] > tmp_hist2[0] * 2){
					loc_half = 0;		// ALL:�ő� HALF:����
				}
				else{
					loc_half = 1;		// �S������
				}
			}
			else{
				if (tmp_hist2[0] < 3 && tmp_hist2[1] < 3){	// HALF�f�[�^�Ȃ�
					loc_half = 0;
					if (tmp_hist1[0] >= 3 && tmp_hist1[0] > tmp_hist1[2] * 2){
						loc_det -= 1;
					}
					if (loc_det <= MINUS){
						loc_half = 1;
					}
				}
				else if (tmp_hist2[0] >= 3 && tmp_hist2[0] > tmp_hist2[2] * 2){
					loc_half = 0;		// ALL:���� HALF:�ŏ�
				}
				else if (tmp_hist1[2] >= 3 && tmp_hist1[2] > tmp_hist1[0] * 2){
					loc_det += 1;		// ALL:�ő� HALF:����
					loc_half = 0;
				}
				else{
					loc_half = 1;		// �S������
				}
			}
		}
		else{					// �t�F�[�h��
			loc_det = loc_max-2;
			int val_max  = hist[loc_max-2];
			if (loc_det == 0){
				loc_det = 1;
				val_max = hist[1];
			}
			for(int i=loc_max-1; i<=loc_max+2; i++){
				int val = hist[i];
				if (val_max * 2 <= val){
					val_max = val;
					loc_det = i;
				}
			}
//printf("(%d,%d:%d,%d,%d,%d,%d)", loc_det,loc_max,
// hist[loc_max-2],hist[loc_max-1],hist[loc_max],hist[loc_max+1],hist[loc_max+2]);
			if (hist[loc_det+1] >= 3 && hist[loc_det-1] * 2 < hist[loc_det+1] &&
				hist[loc_det+1] > hist[loc_det]){
				loc_det += 1;
			}
		}
	}
	//--- ���ʂ��~���b���Z�ŕԂ� ---
	Msec msec_mrg = getMsec30fpsFromFrm(loc_det - MINUS);
	if (loc_half != 0){
		msec_mrg += getMsec30fpsFromFrm(loc_half) / 2;
	}

	return msec_mrg;
}



//---------------------------------------------------------------------
// ���S�ʒu�𖳉��V�[���`�F���W�ʒu����␳
// �ꍇ�ɂ���Ă̓��S�ʒu���疳���V�[���`�F���W�ʒu���C��
//---------------------------------------------------------------------
void JlsReformData::adjustLogo(){
	// ���S�f�[�^�w�肪�Ȃ��ꍇ�A�S�̂����S�f�[�^�Ƃ���
	if (pdata->sizeDataLogo() == 0){
		DataLogoRecord dtlogo;
		pdata->clearRecordLogo(dtlogo);
		dtlogo.fall   = pdata->getMsecTotalMax();
		dtlogo.fall_l = dtlogo.fall;
		dtlogo.fall_r = dtlogo.fall;
		pdata->pushRecordLogo(dtlogo);
		// ���S�ǂݍ��݂Ȃ��ɕύX
		pdata->extOpt.flagNoLogo = 1;
	}

	// ���������s��Ȃ��ꍇ
	if (pdata->extOpt.oldAdjust == 0){
		return;
	}

	int num_logo = pdata->sizeDataLogo();
	for(int i=0; i<num_logo*2; i++){
		int n = jlsd::nlgFromNrf(i);				// logo number
		LogoEdgeType edge = jlsd::edgeFromNrf(i);	// 0:start edge  1:end edge

		DataLogoRecord dtlogo;
		pdata->getRecordLogo(dtlogo, n);

		int msec_lg_c, msec_lg_l, msec_lg_r;
		if (edge == LOGO_EDGE_RISE){
			msec_lg_c = dtlogo.rise;
			msec_lg_l = dtlogo.rise_l;
			msec_lg_r = dtlogo.rise_r;
			dtlogo.stat_rise = LOGO_PRIOR_NONE;			 	// ������
		}
		else{
			msec_lg_c = dtlogo.fall;
			msec_lg_l = dtlogo.fall_l;
			msec_lg_r = dtlogo.fall_r;
			dtlogo.stat_fall = LOGO_PRIOR_NONE;			 	// ������
		}
		msec_lg_l -= pdata->msecValNear1;
		msec_lg_r += pdata->msecValNear1;
		int msec_lgn_l = msec_lg_l - 2*1000;
		int msec_lgn_r = msec_lg_r + 2*1000;

		int lgr_num   = -1;
		LogoPriorType lgr_stat  = LOGO_PRIOR_DUPE;
		int lgr_dif   = -1;
		int lgr_msec  = -1;
		int lgm1_num  = -1;
		int lgm1_dif  = -1;
		int lgm1_msec = -1;
		int lgm2_num  = -1;
		int lgm2_dif  = -1;
		int lgm2_msec = -1;
		int lgn_num   = -1;
		int lgn_dif   = -1;
		int lgn2_num  = -1;
		int lgn2_dif  = -1;
		// ���S�ʒu�ɋ߂������V�[���`�F���W������
		int num_scp = pdata->sizeDataScp();
		for(int j=1; j<num_scp-1; j++){
			DataScpRecord dtscp;
			pdata->getRecordScp(dtscp, j);

			int msec_sc;
			if (edge == LOGO_EDGE_RISE){
				msec_sc = dtscp.msec;
			}
			else{
				msec_sc = dtscp.msbk;
			}
			int msec_smute_s = dtscp.msmute_s - pdata->msecValExact;
			int msec_smute_e = dtscp.msmute_e + pdata->msecValExact;
			LogoPriorType stat_sc = priorLogoFromScp( dtscp.statpos );
			int msec_dif          = abs(msec_sc - msec_lg_c);

//		printf("(%d %d)", stat_sc, frm_sc);
			// ���S��ԓ��ɂ��閳���V�[���`�F���W���o
			if (msec_sc >= msec_lg_l && msec_sc <= msec_lg_r){
				if ((stat_sc >= lgr_stat && (msec_dif < lgr_dif || lgr_dif < 0)) ||
					stat_sc > lgr_stat){
					lgr_num  = j;
					lgr_stat = stat_sc;
					lgr_dif  = msec_dif;
					lgr_msec = msec_sc;
				}
			}
			// �댟�o�����p�̃��S��ԕt�ߖ����V�[���`�F���W���o
			if (msec_sc >= msec_lgn_l && msec_sc <= msec_lgn_r){
				if ((msec_dif < lgn_dif || lgn_dif < 0) && stat_sc <= LOGO_PRIOR_LV1){
					lgn_num = j;
					lgn_dif = msec_dif;
				}
			}
			// �댟�o����2�p�̃��S��ԕt�ߖ����V�[���`�F���W���o
			if (msec_sc >= msec_smute_s && msec_sc <= msec_smute_e){
				if ((msec_dif < lgn2_dif || lgn2_dif < 0) && stat_sc <= LOGO_PRIOR_LV1){
					lgn2_num = j;
					lgn2_dif = msec_dif;
				}
			}
			// �߂��̊m�薳���V�[���`�F���W�ʒu�����o
			if ((msec_dif < lgm1_dif || lgm1_dif < 0) && stat_sc == LOGO_PRIOR_DECIDE){
				lgm1_num  = j;
				lgm1_dif  = msec_dif;
				lgm1_msec = msec_sc;
			}
			// �ׂ̊m�薳���V�[���`�F���W�ʒu�����o
			if ((msec_dif < lgm2_dif || lgm2_dif < 0) && stat_sc == LOGO_PRIOR_DECIDE && msec_dif > pdata->msecValLap2){
				if ((edge == LOGO_EDGE_RISE && msec_sc < msec_lg_c) ||
					(edge != LOGO_EDGE_RISE && msec_sc > msec_lg_c)){
					lgm2_num  = j;
					lgm2_dif  = msec_dif;
					lgm2_msec = msec_sc;
				}
			}
		}
//		printf("%d %d %d %d %d\n", lgr_stat, lgr_frm, lgm1_frm, lgm1_dif, lgm2_frm);
		// ���S�̊m��x
		if (edge == LOGO_EDGE_RISE){
			dtlogo.stat_rise = lgr_stat;
		}
		else{
			dtlogo.stat_fall = lgr_stat;
		}
		// ���S�ʒu��ύX���邩���f
		int flag_lg_change = 0;
		int flag_sc_change = 0;
		int num_sc_change;
		int msec_change;
		// �T�ɖ����V�[���`�F���W���������ꍇ
		if (lgm1_dif > pdata->msecValExact && lgm1_dif <= pdata->msecValNear3 && lgm2_num >= 0){
			CalcDifInfo calc1;
			CalcDifInfo calc2;
			int flag1 = calcDifFunc(calc1, lgm2_msec, msec_lg_c);
			int flag2 = calcDifFunc(calc2, lgm2_msec, lgm1_msec);
			if (flag1 == 1){
				flag1 = 0;
			}
			if (flag2 == 1){
				flag2 = 0;
			}
//			if (flag1 > 0){
//printf("[%d %d]", ncal_sec, ncal2_sec);
//				if (ncal_sec % 5 != 0 && ncal2_sec % 5 == 0){
//					flag1 = 0;
//				}
//			}
			// ���S�͈͓��Ɋm�肵�������V�[���`�F���W���������ꍇ�̏���
			if (lgr_stat == LOGO_PRIOR_DECIDE){
				if (flag1 == 0 && flag2 == 0){		// �����Ƃ��P�ʋ����O�̏ꍇ
					flag2 = 1;						// ���S�𖳉��V�[���`�F���W�ʒu�Ɉڂ�
				}
				else if (flag1 != 0 && flag2 != 0){	// �����Ƃ��P�ʋ������̏ꍇ
					if (calc1.sec != calc2.sec){		// ���Ԃ��S�R�Ⴄ�ꍇ
						flag1 = 0;					// ���S�𖳉��V�[���`�F���W�ʒu�Ɉڂ�
					}
					else{
						calc1.gap += pdata->msecValNear1;		// ���S�������������
					}
				}
			}
			// �m��V�[���`�F���W����P�ʋ����ɂȂ�ʒu�ɋ߂�����I������
			if (flag1 == 0 && flag2 == 0){		// �����Ƃ��P�ʋ����O�̏ꍇ
			}
			else if ((calc1.sec == calc2.sec && calc1.gap > calc2.gap) || flag1 == 0){
				flag_lg_change = 1;
				msec_change    = lgm1_msec;
				lgr_num        = lgm1_num;
			}
			else if ((calc1.sec == calc2.sec && calc1.gap < calc2.gap) || flag2 == 0){
				flag_sc_change = 1;
				num_sc_change  = lgm1_num;
				msec_change    = msec_lg_c;
			}
		}
		else if (lgr_stat == LOGO_PRIOR_DECIDE){	// ���S�͈͓��Ɋm�肵�������V�[���`�F���W���������ꍇ
			flag_lg_change = 1;
			msec_change    = lgr_msec;
		}
		else if (lgn_num >= 0 && lgm2_num >= 0){	// �s�m��^�m��V�[���`�F���W���߂��ɗ�������ꍇ
			CalcDifInfo calc;
			if (calcDifFunc(calc, lgm2_msec, msec_lg_c) > 0){
				if (calc.sec % 15 == 0 || calc.sec == 10){	// ���S���m��V�[���`�F���W����P�ʋ����ɂȂ�ꍇ
					if (lgn_dif > pdata->msecValExact){		// ���S�Ɩ����V�[���`�F���W���אڂ��Ă��Ȃ��ꍇ
						flag_sc_change = 1;
						num_sc_change  = lgn_num;
						msec_change    = msec_lg_c;
					}
				}
			}
		}
		else if (lgn_num >= 0 && edge == LOGO_EDGE_RISE && dtlogo.rise <= 30*1000+400 &&	// �擪�����̃��S
				 (n == 0 || n == 1)){
			if (lgr_num >= 0){										// ��△���V�[���`�F���W����
				if (msec_lg_l <= 0 && (lgr_msec > msec_lg_c * 2)){	// 0�n�_�̕����߂��ꍇ�������Ȃ�
				}
				else{
					flag_lg_change = 1;
					msec_change    = lgr_msec;
				}
			}
			else{											// ���S��ԂɃV�[���`�F���W�Ȃ�
				if (lgn2_num >= 0){							// ���S��Ԃɖ�������
					if (pdata->getScpStatpos(lgn2_num) <= SCP_PRIOR_NONE){
						flag_sc_change = 1;
						num_sc_change  = lgn2_num;
						msec_change    = msec_lg_c;
					}
				}
			}
		}
		// �ŏ��̊J�n�ʒu��₪�s�m�肾�����ꍇ�ɂ͕ύX
		// �ŏ�����J�n�ʒu�ŁA���̃��S��30�b�ȓ��Ɏn�܂�ꍇ���ǉ�
//		if (lgr_num >= 0){
//			if ((n == 0 && edge == LOGO_EDGE_RISE && plogo->nchk_scpos_1st != 0) ||
//				(n == 1 && edge == LOGO_EDGE_RISE && plogo->nchk_scpos_1st != 0 &&
//					pdata->getMsecLogoRise(0) <= 50 && lgr_msec <= 30*1000+400)){
//				plogo->nchk_scpos_1st = lgr_num;
//			}
//		}
		// ���S�ύX���ŏI�V�[���`�F���W����̃t���[���������ꍇ�͕ύX����
		if (flag_lg_change > 0 && msec_change > pdata->getMsecTotalMax()){
			flag_lg_change = 0;
		}
		// ���S�ʒu�̕ύX
		if (flag_lg_change > 0){
			if (edge == LOGO_EDGE_RISE){
				if (dtlogo.rise != msec_change){
					dtlogo.rise = msec_change;
					if (dtlogo.rise_l > msec_change){
						dtlogo.rise_l = msec_change;
					}
					if (dtlogo.rise_r < msec_change){
						dtlogo.rise_r = msec_change;
					}
					int frm_old    = pdata->cnv.getFrmFromMsec(dtlogo.rise);
					int frm_change = pdata->cnv.getFrmFromMsec(msec_change);
					if (abs(frm_old - frm_change) > 2){
						printf("change logo : %d -> %d\n", frm_old, frm_change);
					}
				}
			}
			else{
				if (dtlogo.fall != msec_change){
					dtlogo.fall = msec_change;
					if (dtlogo.fall_l > msec_change){
						dtlogo.fall_l = msec_change;
					}
					if (dtlogo.fall_r < msec_change){
						dtlogo.fall_r = msec_change;
					}
					int frm_old    = pdata->cnv.getFrmFromMsec(dtlogo.fall);
					int frm_change = pdata->cnv.getFrmFromMsec(msec_change);
					if (abs(frm_old - frm_change) > 2){
						printf("change logo : %d -> %d\n", frm_old, frm_change);
					}
				}
			}
		}
		// ���S�f�[�^�X�V
		pdata->setRecordLogo(dtlogo, n);

		// �V�[���`�F���W�ʒu�ύX
		if (flag_sc_change > 0){
			DataScpRecord dtscp;
			pdata->getRecordScp(dtscp, num_sc_change);
			if (edge == LOGO_EDGE_RISE){
				if (dtscp.msec != msec_change){
					int frm_old    = pdata->cnv.getFrmFromMsec(dtscp.msec);
					int frm_change = pdata->cnv.getFrmFromMsec(msec_change);
					int msec_new   = pdata->cnv.getMsecFromFrm(frm_change);
					int msec_newm1 = pdata->cnv.getMsecFromFrm(frm_change-1);
					printf("change scpos : %d -> %d\n", frm_old, frm_change);
					ScpPriorType tmp_stat = dtscp.statpos;
					dtscp.statpos = SCP_PRIOR_DUPE;
					pdata->setRecordScp(dtscp, num_sc_change);
					pdata->insertScpos(msec_new, msec_newm1, num_sc_change, tmp_stat);
				}
			}
			else{
				if (dtscp.msbk != msec_change){
					int frm_old    = pdata->cnv.getFrmFromMsec(dtscp.msec);
					int frm_change = pdata->cnv.getFrmFromMsec(msec_change);
					int msec_new   = pdata->cnv.getMsecFromFrm(frm_change);
					int msec_newp1 = pdata->cnv.getMsecFromFrm(frm_change+1);
					printf("change scpos : %d -> %d\n", frm_old, frm_change);
					ScpPriorType tmp_stat = dtscp.statpos;
					dtscp.statpos = SCP_PRIOR_DUPE;
					pdata->setRecordScp(dtscp, num_sc_change);
					pdata->insertScpos(msec_newp1, msec_new, num_sc_change, tmp_stat);
				}
			}
		}
	}
}



//---------------------------------------------------------------------
// �Q�t���[���Ԃ̕b�����擾
// ���́F
//   frm_src  : ��r���t���[���ԍ�
//   frm_dst  : ��r��t���[���ԍ�
// �o�́F
//   calc.sgn : ��r�t���[���召
//   calc.sec : �t���[���ԕb��
//   calc.gap : �t���[���ԕb���̌덷
// �Ԃ�l�F
//   2 : 1�̏����𖞂�������25�b�ȉ��܂���30,45,60,90,120�b
//   1 : �R�b�ȏォ�b�P�ʂŌ덷�����Ȃ��ꍇ�i�P�T�b�P�ʂł͌덷�Â߁j
//   0 : ����ȊO
//---------------------------------------------------------------------
int JlsReformData::calcDifFunc(CalcDifInfo &calc, int msec_src, int msec_dst){
	int flag = 0;
	Msec msec_dif_org = msec_dst - msec_src;
	Msec msec_dif_abs = abs(msec_dif_org);
	Sec  sec_dif      = (msec_dif_abs + 500) / 1000;
	Msec msec_gap     = abs(sec_dif * 1000 - msec_dif_abs);
	if ((msec_gap <= pdata->msecValNear1 && sec_dif >= 3) ||						// 3�b�ȏ�ŕb�P�ʐؑ�
		(msec_gap <= pdata->msecValNear2 && sec_dif == 10) ||						// 10�b
		(msec_gap <= pdata->msecValNear2 && sec_dif >= 15 && sec_dif % 15 == 0)){	// 15�b�P�ʐ؂�ւ�
		if (sec_dif <= 120){
			flag = 1;
			if (sec_dif <= 25 || sec_dif == 30 || sec_dif == 45 || sec_dif == 60 ||
				sec_dif == 90 || sec_dif == 120){
				flag = 2;
			}
		}
	}
	// ���ʊi�[
	if (msec_dif_org < 0 && sec_dif != 0){
		calc.sgn = -1;
	}
	else if (msec_dif_org > 0 && sec_dif != 0){
		calc.sgn = 1;
	}
	else{
		calc.sgn = 0;
	}
	calc.sec = sec_dif;
	calc.gap = msec_gap;

	return flag;
}


//---------------------------------------------------------------------
// �Q�t���[���Ԃ̕b�����擾�i�덷���Ȃ��O��j
// ���́F
//   frm_src  : ��r���t���[���ԍ�
//   frm_dst  : ��r��t���[���ԍ�
// �o�́F
//   ncal_sgn : ��r�t���[���召
//   ncal_sec : �t���[���ԕb��
//   ncal_dis : �t���[���ԕb���̌덷
// �Ԃ�l�F
//   2 : 10,15,30,45,60,90,120�b
//   1 : �R�b�ȏ�Q�T�b�ȉ����b�P�ʂŌ덷�����Ȃ��ꍇ
//   0 : ����ȊO
//---------------------------------------------------------------------
int JlsReformData::calcDifExact(CalcDifInfo &calc, int msec_src, int msec_dst){
	int flag = 0;
	Msec msec_dif_org = msec_dst - msec_src;
	Msec msec_dif_abs = abs(msec_dif_org);
	Sec  sec_dif      = (msec_dif_abs + 500) / 1000;
	Msec msec_gap     = abs(sec_dif * 1000 - msec_dif_abs);
	if (msec_gap <= pdata->msecValNear1 &&
		(sec_dif == 10 || sec_dif == 15 || sec_dif == 30 || sec_dif == 45 ||
		 sec_dif == 60 || sec_dif == 90 || sec_dif == 120)){
		flag = 2;
	}
	else if (msec_gap <= pdata->msecValExact && (sec_dif >= 3 && sec_dif <= 25)){
		flag = 1;
	}
	// ���ʊi�[
	if (msec_dif_org < 0 && sec_dif != 0){
		calc.sgn = -1;
	}
	else if (msec_dif_org > 0 && sec_dif != 0){
		calc.sgn = 1;
	}
	else{
		calc.sgn = 0;
	}
	calc.sec = sec_dif;
	calc.gap = msec_gap;

	return flag;
}

//---------------------------------------------------------------------
// 33ms��1�Ƃ����P�ʂɕϊ�
//---------------------------------------------------------------------
int JlsReformData::getFrm30fpsFromMsec(int msec){
	int frate_n = 30000;
	int frate_d = 1001;
	int r = ((((long long)abs(msec) * frate_n) + (frate_d*1000/2)) / (frate_d*1000));
	return (msec >= 0)? r : -r;
}
int JlsReformData::getMsec30fpsFromFrm(int frm){
	int frate_n = 30000;
	int frate_d = 1001;
	int r = (((long long)abs(frm) * frate_d * 1000 + (frate_n/2)) / frate_n);
	return (frm >= 0)? r : -r;
}

