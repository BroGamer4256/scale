#include "diva.h"

using namespace diva;

struct ScaleDb {
	i32 state;
	u64 romDir;
	void *fileHandler;
	std::map<i32, f32> charaScale;
	std::map<std::pair<i32, i32>, f32> cosScale;
} scale_db;

HOOK (bool, PvDbRead, 0x1404BB290, u64 task) {
	auto res = originalPvDbRead (task);

	if (*(i32 *)(task + 0x68) == 0 && scale_db.state != 4) {
		switch (scale_db.state) {
		case 0: {
			auto dir = romDirs->at (scale_db.romDir);
			if (!dir.has_value ()) {
				scale_db.state = 4;
				break;
			}

			auto romDir = dir.value ();
			if (strcmp (romDir->c_str (), "./") == 0) {
				scale_db.romDir++;
				break;
			}

			char buf[MAX_PATH];
			sprintf (buf, "%s/scale_db.toml", romDir->c_str ());
			auto str = string (buf);
			if (!ResolveFilePath (&str, nullptr)) {
				scale_db.romDir++;
				break;
			}

			FileRequestLoad (&scale_db.fileHandler, buf, true);
			scale_db.state = 1;
		}
		case 1: {
			if (FileCheckNotReady (&scale_db.fileHandler)) break;
			scale_db.state = 2;
		}
		case 2: {
			auto data = (char *)FileGetData (&scale_db.fileHandler);
			auto size = FileGetSize (&scale_db.fileHandler);
			if (!data || size == 0) {
				scale_db.state = 0;
				scale_db.romDir++;
				break;
			}
			data[size - 1] = 0;

			char errbuf[255];
			auto toml = toml_parse (data, errbuf, 255);
			if (toml) {
				auto arr = toml_array_in (toml, "chara_scale");
				if (arr) {
					for (auto i = 0;; i++) {
						auto tbl = toml_table_at (arr, i);
						if (!tbl) break;
						auto chara = toml_int_in (tbl, "chara");
						auto scale = toml_double_in (tbl, "scale");
						if (!chara.ok || !scale.ok) continue;

						scale_db.charaScale[(i32)chara.u.i] = (f32)scale.u.d;
					}
				}

				arr = toml_array_in (toml, "cos_scale");
				if (arr) {
					for (auto i = 0;; i++) {
						auto tbl = toml_table_at (arr, i);
						if (!tbl) break;
						auto chara = toml_int_in (tbl, "chara");
						auto cos   = toml_int_in (tbl, "cos");
						auto scale = toml_double_in (tbl, "scale");
						if (!chara.ok || !cos.ok || !scale.ok) continue;

						scale_db.cosScale[std::pair ((i32)chara.u.i, (i32)cos.u.i)] = (f32)scale.u.d;
					}
				}

				toml_free (toml);
			} else {
				printf ("Failed to parse %s: %s\n", romDirs->at (scale_db.romDir).value ()->c_str (), errbuf);
			}

			FreeFileHandler (scale_db.fileHandler);
			scale_db.state = 0;
			scale_db.romDir++;
			break;
		}
		}
	}

	return res;
}

HOOK (i64, PvGamePvDataCtrl, 0x14024EB50, u64 a1, f32 delta, i64 curr, bool a4) {
	auto ret = originalPvGamePvDataCtrl (a1, delta, curr, a4);
	for (auto i = 0; i < 6; i++) {
		auto rob_chara = *(u64 *)(a1 + 0x2BFF0 + (i * 0xA0));
		if (rob_chara == 0) continue;
		auto chara_id = *(i32 *)(rob_chara + 0x10);
		auto cos_id   = *(i32 *)(rob_chara + 0x14);

		auto cos = std::pair (chara_id, cos_id);
		if (scale_db.cosScale.contains (cos)) {
			auto scale                    = scale_db.cosScale[cos];
			*(f32 *)(rob_chara + 0x2224)  = scale;
			*(bool *)(rob_chara + 0x2228) = scale != 1.0;
			*(f32 *)(rob_chara + 0x222C)  = scale - 1.0;
		} else if (scale_db.charaScale.contains (chara_id)) {
			auto scale                    = scale_db.charaScale[chara_id];
			*(f32 *)(rob_chara + 0x2224)  = scale;
			*(bool *)(rob_chara + 0x2228) = scale != 1.0;
			*(f32 *)(rob_chara + 0x222C)  = scale - 1.0;
		}
	}

	return ret;
}

extern "C" {
__declspec (dllexport) void
init () {
	scale_db.state  = 0;
	scale_db.romDir = 0;

	INSTALL_HOOK (PvDbRead);
	INSTALL_HOOK (PvGamePvDataCtrl);
}
}
