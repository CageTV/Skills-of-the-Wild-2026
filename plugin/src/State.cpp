/*
 * Skills of the Wild 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#include "PCH.h"
#include "State.h"

namespace State
{
	namespace
	{
		constexpr std::uint32_t kUniqueId = 'SOTW';
		constexpr std::uint32_t kRecord = 'STAT';
		constexpr std::uint32_t kVersion = 1;

		Data data;

		void OnSave(SKSE::SerializationInterface* a_intfc)
		{
			if (a_intfc->OpenRecord(kRecord, kVersion)) {
				a_intfc->WriteRecordData(data.lastDay);
				a_intfc->WriteRecordData(data.lastCleans);
			}
		}

		void OnLoad(SKSE::SerializationInterface* a_intfc)
		{
			data = {};
			std::uint32_t type, version, length;
			while (a_intfc->GetNextRecordInfo(type, version, length)) {
				if (type == kRecord && version == kVersion) {
					a_intfc->ReadRecordData(data.lastDay);
					a_intfc->ReadRecordData(data.lastCleans);
				}
			}
			SKSE::log::info("Co-save loaded: last day {}, last hunting experience {}", data.lastDay, data.lastCleans);
		}

		void OnRevert(SKSE::SerializationInterface*) { data = {}; }
	}

	Data& D() { return data; }

	void Register()
	{
		auto* ser = SKSE::GetSerializationInterface();
		ser->SetUniqueID(kUniqueId);
		ser->SetSaveCallback(OnSave);
		ser->SetLoadCallback(OnLoad);
		ser->SetRevertCallback(OnRevert);
	}
}

namespace Journal
{
	namespace
	{
		std::mutex               mu;
		std::vector<std::string> lines;
	}

	void Add(const std::string& a_line)
	{
		SKSE::log::info("{}", a_line);
		std::lock_guard lock(mu);
		lines.push_back(a_line);
		if (lines.size() > 12) {
			lines.erase(lines.begin());
		}
	}

	std::vector<std::string> Recent()
	{
		std::lock_guard lock(mu);
		return lines;
	}
}
