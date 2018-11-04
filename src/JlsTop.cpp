// コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "JlsIF.hpp"

//---------------------------------------------------------------------
// join_logo_scp
// 返り値：
//  0 : 正常終了
//  1 : 何も実行しないで終了
//  2 : 設定・ファイルエラー
//  3 : 実行エラー
//---------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int ret;
	try{
		JlsIF jls;
		jls.setArgFull(argc, argv);		// 引数読み込み
		ret = jls.runScript();			// 実行
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
