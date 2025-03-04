#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
#include <ranges>
#include "Memory.hpp"
#include "SkinDatabase.hpp"
#include "CheatManager.hpp"
#include "GUI.hpp"
#include "Utils.hpp"
#include "fnv_hash.hpp"
#include "imgui/imgui.h"

static void changeTurretSkin(const std::int32_t skinId, const std::int32_t team) noexcept
{
	if (skinId == -1)
		return;

	const auto turrets{ cheatManager.memory->turretList };
	const auto playerTeam{ cheatManager.memory->localPlayer->get_team() };

	for (auto i{ 0u }; i < turrets->length; ++i) {
		if (const auto turret{ turrets->list[i] }; turret->get_team() == team) {
			if (playerTeam == team) {
				turret->get_character_data_stack()->base_skin.skin = skinId * 2;
				turret->get_character_data_stack()->update(true);
			} else {
				turret->get_character_data_stack()->base_skin.skin = skinId * 2 + 1;
				turret->get_character_data_stack()->update(true);
			}
		}
	}
}

void GUI::render() noexcept
{
	static bool updatedStyle = false;
	if (!updatedStyle) {
		ImGuiIO &io = ImGui::GetIO();
		// style.Alpha = 0.75f;
		ImGuiStyle &style = ImGui::GetStyle();
		// Set up fonts
		ImFontConfig config;
		config.OversampleH = 3;
		config.OversampleV = 3;
		config.PixelSnapH = true;
		//io.Fonts->AddFontFromFileTTF("HelveticaNeue-Regular.otf", 15.f, &config);
		style.WindowRounding = 2.0f;
		style.FrameRounding = 2.0f;
		style.ScrollbarRounding = 2.0f;
		style.GrabRounding = 2.0f;
		style.TabRounding = 2.0f;

		// Colors
		ImVec4 *colors = style.Colors;

		// Window
		colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.15f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.11f, 0.11f, 0.18f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.11f, 0.11f, 0.15f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.15f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);

		// Text
		colors[ImGuiCol_Text] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

		// Buttons
		colors[ImGuiCol_Button] = ImVec4(0.24f, 0.24f, 0.28f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);

		// Header
		colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);

		// Input fields
		colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		// Menus
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.15f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.15f, 0.98f);

		// Separators
		colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

		// Resize grip
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

		// Drag and drop
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.90f);
	}
	std::call_once(set_font_scale, [&]
	{
		ImGui::GetIO().FontGlobalScale = cheatManager.config->fontScale;
	});

	const auto player{ cheatManager.memory->localPlayer };
	const auto heroes{ cheatManager.memory->heroList };
	static const auto my_team{ player ? player->get_team() : 100 };
	static int gear{ player ? player->get_character_data_stack()->base_skin.gear : 0 };

	static const auto vector_getter_skin = [](void* vec, const std::int32_t idx, const char** out_text) noexcept {
		const auto& vector{ *static_cast<std::vector<SkinDatabase::skin_info>*>(vec) };
		if (idx < 0 || idx > static_cast<std::int32_t>(vector.size())) return false;
		*out_text = idx == 0 ? "Default" : vector.at(idx - 1).skin_name.c_str();
		return true;
	};

	static const auto vector_getter_ward_skin = [](void* vec, const std::int32_t idx, const char** out_text) noexcept {
		const auto& vector{ *static_cast<std::vector<std::pair<std::int32_t, const char*>>*>(vec) };
		if (idx < 0 || idx > static_cast<std::int32_t>(vector.size())) return false;
		*out_text = idx == 0 ? "Default" : vector.at(idx - 1).second;
		return true;
	};

	static auto vector_getter_gear = [](void* vec, const std::int32_t idx, const char** out_text) noexcept {
		const auto& vector{ *static_cast<std::vector<const char*>*>(vec) };
		if (idx < 0 || idx > static_cast<std::int32_t>(vector.size())) return false;
		*out_text = vector[idx];
		return true;
	};

	static auto vector_getter_default = [](void* vec, const std::int32_t idx, const char** out_text) noexcept {
		const auto& vector{ *static_cast<std::vector<const char*>*>(vec) };
		if (idx < 0 || idx > static_cast<std::int32_t>(vector.size())) return false;
		*out_text = idx == 0 ? "Default" : vector.at(idx - 1);
		return true;
	};

	ImGui::Begin("2PACALYPSE 2.3 - MONEYMACK", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize);
	{
		ImGui::rainbowText();
		if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip)) {
			if (player) {
				if (ImGui::BeginTabItem("Local Player")) {
					auto& values{ cheatManager.database->champions_skins[fnv::hash_runtime(player->get_character_data_stack()->base_skin.model.str)] };
					ImGui::Text("Player Skins Settings:");

					if (ImGui::Combo("Current Skin", &cheatManager.config->current_combo_skin_index, vector_getter_skin, static_cast<void*>(&values), values.size() + 1))
						if (cheatManager.config->current_combo_skin_index > 0)
							player->change_skin(values[cheatManager.config->current_combo_skin_index - 1].model_name, values[cheatManager.config->current_combo_skin_index - 1].skin_id);
					
					const auto playerHash{ fnv::hash_runtime(player->get_character_data_stack()->base_skin.model.str) };
					if (const auto it{ std::ranges::find_if(cheatManager.database->specialSkins,
					[& skin = player->get_character_data_stack()->base_skin.skin, &ph = playerHash](const SkinDatabase::specialSkin& x) noexcept -> bool
						{
						   return x.champHash == ph && (x.skinIdStart <= skin && x.skinIdEnd >= skin);
						})
					}; it != cheatManager.database->specialSkins.end())
					{
						const auto stack{ player->get_character_data_stack() };
						gear = stack->base_skin.gear;

						if (ImGui::Combo("Current Gear", &gear, vector_getter_gear, static_cast<void*>(&it->gears), it->gears.size())) {
							player->get_character_data_stack()->base_skin.gear = static_cast<std::int8_t>(gear);
							player->get_character_data_stack()->update(true);
						}
						ImGui::Separator();
					}

					if (ImGui::Combo("Current Ward Skin", &cheatManager.config->current_combo_ward_index, vector_getter_ward_skin, static_cast<void*>(&cheatManager.database->wards_skins), cheatManager.database->wards_skins.size() + 1))
						cheatManager.config->current_ward_skin_index = cheatManager.config->current_combo_ward_index == 0 ? -1 : cheatManager.database->wards_skins.at(cheatManager.config->current_combo_ward_index - 1).first;
					ImGui::EndTabItem();
				}
			}

			if (heroes->length > 1) {
				if (ImGui::BeginTabItem("Other Champs")) {
					ImGui::Text("Other Champs Skins Settings:");
					std::int32_t last_team { 0 };
					for (auto i { 0u }; i < heroes->length; ++i) {
						const auto hero { heroes->list[i] };
	
						if (hero == player)
							continue;
	
						const auto champion_name_hash { fnv::hash_runtime(hero->get_character_data_stack()->base_skin.model.str) };
						if (champion_name_hash == FNV("PracticeTool_TargetDummy"))
							continue;
	
						const auto hero_team { hero->get_team() };
						const auto is_enemy { hero_team != my_team };
	
						if (last_team == 0 || hero_team != last_team) {
							if (last_team != 0)
								ImGui::Separator();
							if (is_enemy)
								ImGui::Text(" Enemy champions");
							else
								ImGui::Text(" Ally champions");
							last_team = hero_team;
						}
	
						auto& config_array { is_enemy ? cheatManager.config->current_combo_enemy_skin_index : cheatManager.config->current_combo_ally_skin_index };
						const auto [fst, snd] { config_array.insert( { champion_name_hash, 0 }) };
	
						std::snprintf(this->str_buffer, sizeof(this->str_buffer), cheatManager.config->heroName ? "HeroName: [ %s ]##%X" : "PlayerName: [ %s ]##%X", cheatManager.config->heroName ? hero->get_character_data_stack()->base_skin.model.str : hero->get_name()->c_str(), reinterpret_cast<std::uintptr_t>(hero));
	
						auto& values { cheatManager.database->champions_skins[champion_name_hash] };
						if (ImGui::Combo(str_buffer, &fst->second, vector_getter_skin, static_cast<void*>(&values), values.size() + 1))
							if (fst->second > 0)
								hero->change_skin(values[fst->second - 1].model_name, values[fst->second - 1].skin_id);
					}
					ImGui::EndTabItem();
				}
			}

			if (ImGui::BeginTabItem("Global Skins")) {
				ImGui::Text("Global Skins Settings:");
				if (ImGui::Combo("Minion Skins:", &cheatManager.config->current_combo_minion_index, vector_getter_default, static_cast<void*>(&cheatManager.database->minions_skins), cheatManager.database->minions_skins.size() + 1))
					cheatManager.config->current_minion_skin_index = cheatManager.config->current_combo_minion_index - 1;
				ImGui::Separator();
				if (ImGui::Combo("Order Turret Skins:", &cheatManager.config->current_combo_order_turret_index, vector_getter_default, static_cast<void*>(&cheatManager.database->turret_skins), cheatManager.database->turret_skins.size() + 1))
					changeTurretSkin(cheatManager.config->current_combo_order_turret_index - 1, 100);
				if (ImGui::Combo("Chaos Turret Skins:", &cheatManager.config->current_combo_chaos_turret_index, vector_getter_default, static_cast<void*>(&cheatManager.database->turret_skins), cheatManager.database->turret_skins.size() + 1))
					changeTurretSkin(cheatManager.config->current_combo_chaos_turret_index - 1, 200);
				ImGui::Separator();
				ImGui::Text("Jungle Mobs Skins Settings:");
				for (auto& it : cheatManager.database->jungle_mobs_skins) {
					std::snprintf(str_buffer, 256, "Current %s skin", it.name);
					const auto [fst, snd]{ cheatManager.config->current_combo_jungle_mob_skin_index.insert({ it.name_hashes.front(), 0 }) };
					if (ImGui::Combo(str_buffer, &fst->second, vector_getter_default, &it.skins, it.skins.size() + 1))
						for (const auto& hash : it.name_hashes)
							cheatManager.config->current_combo_jungle_mob_skin_index[hash] = fst->second;
				}
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Logger")) {
				cheatManager.logger->draw();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Extras")) {
				ImGui::hotkey("Menu Key", cheatManager.config->menuKey);
				ImGui::Checkbox(cheatManager.config->heroName ? "HeroName based" : "PlayerName based", &cheatManager.config->heroName);
				ImGui::Checkbox("Rainbow Text", &cheatManager.config->rainbowText);
				ImGui::Checkbox("Quick Skin Change", &cheatManager.config->quickSkinChange);
				ImGui::hoverInfo("It allows you to change skin without opening the menu with the key you assign from the keyboard.");

				if (cheatManager.config->quickSkinChange) {
					ImGui::Separator();
					ImGui::hotkey("Previous Skin Key", cheatManager.config->previousSkinKey);
					ImGui::hotkey("Next Skin Key", cheatManager.config->nextSkinKey);
					ImGui::Separator();
				}

				if (player)
					ImGui::InputText("Change Nick", player->get_name());

				if (ImGui::Button("No skins except local player")) {
					for (auto& val : cheatManager.config->current_combo_enemy_skin_index | std::views::values)
						val = 1;

					for (auto& val : cheatManager.config->current_combo_ally_skin_index | std::views::values)
						val = 1;

					for (auto i{ 0u }; i < heroes->length; ++i) {
						if (const auto hero{ heroes->list[i] }; hero != player)
							hero->change_skin(hero->get_character_data_stack()->base_skin.model.str, 0);
					}
				} ImGui::hoverInfo("Sets the skins of all champions except the local player to the default skin.");

				if (ImGui::Button("Random Skins")) {
					for (auto i{ 0u }; i < heroes->length; ++i) {
						const auto hero{ heroes->list[i] };
						const auto championHash{ fnv::hash_runtime(hero->get_character_data_stack()->base_skin.model.str) };
						
						if (championHash == FNV("PracticeTool_TargetDummy"))
							continue;
						
						const auto skinCount{ cheatManager.database->champions_skins[championHash].size() };
						auto& skinDatabase{ cheatManager.database->champions_skins[championHash] };
						auto& config{ (hero->get_team() != my_team) ? cheatManager.config->current_combo_enemy_skin_index : cheatManager.config->current_combo_ally_skin_index };

						if (hero == player) {
							cheatManager.config->current_combo_skin_index = random(1ull, skinCount);
							hero->change_skin(skinDatabase[cheatManager.config->current_combo_skin_index - 1].model_name, skinDatabase[cheatManager.config->current_combo_skin_index - 1].skin_id);
						} else {
							auto& data{ config[championHash] };
							data = random(1ull, skinCount);
							hero->change_skin(skinDatabase[data - 1].model_name, skinDatabase[data - 1].skin_id);
						}
					}
				} ImGui::hoverInfo("Randomly changes the skin of all champions.");

				ImGui::SliderFloat("Font Scale", &cheatManager.config->fontScale, 1.0f, 2.0f, "%.3f");
				if (ImGui::GetIO().FontGlobalScale != cheatManager.config->fontScale) {
					ImGui::GetIO().FontGlobalScale = cheatManager.config->fontScale;
				} ImGui::hoverInfo("Changes the menu font scale.");
				
				if (ImGui::Button("Force Close"))
					cheatManager.hooks->uninstall();
				ImGui::hoverInfo("You will be returned to the reconnect screen.");
				ImGui::Text("FPS: %.0f FPS", ImGui::GetIO().Framerate);
				ImGui::EndTabItem();
			}
		}
	}
	ImGui::End();
}
