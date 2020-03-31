#include<lbpch.h>
#include<I18N.h>
#include<JsonLoader.h>
namespace I18N {
	LIGHTBASE_API string CMD_SUCCESS;
	LIGHTBASE_API string CMD_EXCEPTION;
	LIGHTBASE_API string EVENT_EXCEPTION_S;
	LIGHTBASE_API string S_TARGET;
	LIGHTBASE_API string S_OPERATION;
	void InitAll() {
		try {
			ConfigJReader jr("langpack/global.json");
#define bindLANG(x) jr.bind(#x, x)
			bindLANG(EVENT_EXCEPTION_S);
			bindLANG(CMD_EXCEPTION);
			bindLANG(CMD_SUCCESS);
			bindLANG(S_TARGET);
			bindLANG(S_OPERATION);
		}
		catch (string e) {
			LOG.p<LOGLVL::Error>(" I18N file load error ", e);
		}
	}
}
