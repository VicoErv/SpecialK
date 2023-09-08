/**
 * This file is part of Special K.
 *
 * Special K is free software : you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by The Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Special K is distributed in the hope that it will be useful,
 *
 * But WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Special K.
 *
 *   If not, see <http://www.gnu.org/licenses/>.
 *
**/

#include <SpecialK/stdafx.h>
#include <SpecialK/control_panel/plugins.h>
#include <imgui/imfilebrowser.h>

#include <filesystem>

extern iSK_INI* dll_ini;

using namespace SK::ControlPanel;
bool
SK_ImGui_SavePlugInPreference (iSK_INI* ini, bool enable, const wchar_t* import_name, const wchar_t* role, SK_Import_LoadOrder order, const wchar_t* path)
{
  if (! ini)
    return false;

  if (! enable)
  {
    ini->remove_section (import_name);
    ini->write          ();

    return true;
  }

  if (GetFileAttributesW (path) != INVALID_FILE_ATTRIBUTES)
  {
    wchar_t wszImportRecord [4096] = { };

    swprintf ( wszImportRecord, L"[%s]\n"
#ifdef _WIN64
                                 L"Architecture=x64\n"
#else
                                 L"Architecture=Win32\n"
#endif
                                 L"Role=%s\n"
                                 L"When=%s\n"
                                 L"Filename=%s\n\n",
                                   import_name,
                                     role,
      order == SK_Import_LoadOrder::Early  ? L"Early"  :
      order == SK_Import_LoadOrder::PlugIn ? L"PlugIn" :
                                             L"Lazy",
                                         path );

    ini->import (wszImportRecord);
    ini->write  ();

    return true;
  }

  return false;
}


void
SK_ImGui_PlugInDisclaimer (void)
{
  ImGui::PushStyleColor (ImGuiCol_Text, (ImVec4&&)ImColor::HSV (0.15f, 0.95f, 0.98f));
  ImGui::TextWrapped    ("If you run into problems with a Plug-In, pressing and holding Ctrl + Shift at game startup can disable them.");
  ImGui::PopStyleColor  ();
}

bool
SK_ImGui_PlugInSelector (iSK_INI* ini, const std::string& name, const wchar_t* path, const wchar_t* import_name, bool& enable, SK_Import_LoadOrder& order, SK_Import_LoadOrder default_order)
{
  static float max_name_width = 0.0f;

  if (! ini)
    return false;

  std::string hash_name  = name + "##PlugIn";
  std::string hash_load  = "Load Order##";
              hash_load += name;

  float cursor_x =
    ImGui::GetCursorPosX ();

  bool changed =
    ImGui::Checkbox (hash_name.c_str (), &enable);

  ImGui::SameLine ();

  max_name_width =
    std::max (max_name_width, ImGui::GetCursorPosX () - cursor_x);

  if (ImGui::IsItemHovered ())
  {
    if (GetFileAttributesW (path) == INVALID_FILE_ATTRIBUTES)
      ImGui::SetTooltip ("Please install %s to %hs", name.c_str (), std::filesystem::path (path).c_str ());
  }

  if (ini->contains_section (import_name))
  {
    if (     ini->get_section (import_name).get_value (L"When") == L"Early")
      order = SK_Import_LoadOrder::Early;
    else if (ini->get_section (import_name).get_value (L"When") == L"PlugIn")
      order = SK_Import_LoadOrder::PlugIn;
    else
      order = SK_Import_LoadOrder::Lazy;
  }
  else
    order = default_order;

  ImGui::SameLine      ();
  ImGui::SetCursorPosX (cursor_x + max_name_width);

  int iOrder =
    static_cast <int> (order);

  changed |=
    ImGui::Combo (hash_load.c_str (), &iOrder, "Early\0Plug-In\0Lazy\0\0");

  if (changed)
    order = static_cast <SK_Import_LoadOrder> (iOrder);

  if (ImGui::IsItemHovered ())
  {
    ImGui::BeginTooltip ();
    if (! SK_IsInjected ())
      ImGui::Text       ("Plug-In Load Order is Suggested by Default.");
    else
      ImGui::Text       ("Lazy Load Order is Suggested by Default.");
    ImGui::Separator    ();
    ImGui::BulletText   ("If a plug-in does not show up or the game crashes, try loading it early.");
    ImGui::BulletText   ("Early plug-ins handle rendering before Special K; ReShade will apply its effects to Special K's UI if loaded early.");
    ImGui::BulletText   ("Lazy plug-ins have undefined load order, but may allow ReShade to load as a plug-in in some stubborn games.");
    ImGui::EndTooltip   ();
  }

  return changed;
}


bool
SK::ControlPanel::PlugIns::Draw (void)
{
  if (ImGui::CollapsingHeader ("Plug-Ins"))
  {
    ImGui::TreePush ("");

    static bool bUnity =
      SK_GetCurrentRenderBackend ().windows.unity;

    wchar_t imp_path_reshade    [MAX_PATH + 2] = { };
    wchar_t imp_name_reshade    [64]           = { };

    wchar_t imp_path_reshade_ex [MAX_PATH + 2] = { };
    wchar_t imp_name_reshade_ex [64]           = { };

#ifdef _WIN64
    wcscat   (imp_name_reshade, L"Import.ReShade64");
    swprintf (imp_path_reshade, LR"(%ws\PlugIns\ThirdParty\ReShade\ReShade64.dll)",
                                SK_GetInstallPath ());

    wcscat   (imp_name_reshade_ex, L"Import.ReShade64_Custom");

    if (SK_IsInjected ())
    {
      swprintf (imp_path_reshade_ex, LR"(%ws\PlugIns\Unofficial\ReShade\ReShade64.dll)",
                                     SK_GetInstallPath ());
    }

    else
    {
      static constexpr wchar_t* wszShimFormat =
        LR"(%ws\PlugIns\Unofficial\ReShade\ReShade%u.dll)";

      swprintf (imp_path_reshade_ex,  wszShimFormat,
                SK_GetInstallPath (), SK_GetBitness ());
    }
#else
    wcscat   (imp_name_reshade, L"Import.ReShade32");
    swprintf (imp_path_reshade, LR"(%ws\PlugIns\ThirdParty\ReShade\ReShade32.dll)",
                                SK_GetInstallPath ());

    wcscat   (imp_name_reshade_ex, L"Import.ReShade32_Custom");

    if (SK_IsInjected ())
    {
      swprintf (imp_path_reshade_ex, LR"(%ws\PlugIns\Unofficial\ReShade\ReShade32.dll)",
                                     SK_GetInstallPath ());
    }

    else
    {
      static constexpr wchar_t* wszShimFormat =
        LR"(%s\PlugIns\Unofficial\ReShade\ReShade%u.dll)";

      swprintf (imp_path_reshade_ex,  wszShimFormat,
                SK_GetInstallPath (), SK_GetBitness ());
    }
#endif
    bool reshade_official   = dll_ini->contains_section (imp_name_reshade);
    bool reshade_unofficial = dll_ini->contains_section (imp_name_reshade_ex);

    static SK_Import_LoadOrder order    = SK_Import_LoadOrder::Early;
    static SK_Import_LoadOrder order_ex = SK_Import_LoadOrder::PlugIn;

    bool changed = false;

    ImGui::PushStyleColor (ImGuiCol_Header,        ImVec4 (0.90f, 0.68f, 0.02f, 0.45f));
    ImGui::PushStyleColor (ImGuiCol_HeaderHovered, ImVec4 (0.90f, 0.72f, 0.07f, 0.80f));
    ImGui::PushStyleColor (ImGuiCol_HeaderActive,  ImVec4 (0.87f, 0.78f, 0.14f, 0.80f));

    if (ImGui::CollapsingHeader ("Third-Party"))
    {
      ImGui::TreePush    ("");
      changed |=
          SK_ImGui_PlugInSelector (
            dll_ini, "ReShade (Official)", imp_path_reshade, imp_name_reshade, reshade_official, order,
              SK_IsInjected () ? SK_Import_LoadOrder::Lazy :
                                 SK_Import_LoadOrder::PlugIn );

      ImGui::TreePop     (  );
    }
    ImGui::PopStyleColor ( 3);

    if (SK_IsInjected () || StrStrIW (SK_GetModuleName (SK_GetDLL ()).c_str (), L"dxgi.dll")     ||
                            StrStrIW (SK_GetModuleName (SK_GetDLL ()).c_str (), L"d3d11.dll")    ||
                            StrStrIW (SK_GetModuleName (SK_GetDLL ()).c_str (), L"d3d9.dll")     ||
                            StrStrIW (SK_GetModuleName (SK_GetDLL ()).c_str (), L"OpenGL32.dll") ||
                            StrStrIW (SK_GetModuleName (SK_GetDLL ()).c_str (), L"dinput8.dll"))
    {
      ImGui::PushStyleColor (ImGuiCol_Header,        ImVec4 (0.02f, 0.68f, 0.90f, 0.45f));
      ImGui::PushStyleColor (ImGuiCol_HeaderHovered, ImVec4 (0.07f, 0.72f, 0.90f, 0.80f));
      ImGui::PushStyleColor (ImGuiCol_HeaderActive,  ImVec4 (0.14f, 0.78f, 0.87f, 0.80f));

      const bool unofficial =
        ImGui::CollapsingHeader ("Unofficial");

      ImGui::PushStyleColor (ImGuiCol_Text, (ImVec4&&)ImColor::HSV (.247f, .95f, .98f));
      ImGui::SameLine       ();
      ImGui::Text           ("           Customized for Special K");
      ImGui::PopStyleColor  ();

      if (unofficial)
      {
        ImGui::TreePush    ("");
        changed |=
            SK_ImGui_PlugInSelector (dll_ini, "ReShade (Deprecated!!)", imp_path_reshade_ex, imp_name_reshade_ex, reshade_unofficial, order_ex);
        ImGui::TreePop     (  );
      }
      ImGui::PopStyleColor ( 3);
    }

    struct import_s {
      wchar_t             path     [MAX_PATH + 2] = { };
      wchar_t             ini_name [64]           = L"Import.";
      std::string         label                   = "";
      std::string         version                 = "";
      SK_Import_LoadOrder order                   = SK_Import_LoadOrder::PlugIn;
      bool                enabled                 = true;
      bool                loaded                  = false;
    };

    static ImGui::FileBrowser                          fileDialog (ImGuiFileBrowserFlags_MultipleSelection);
    static std::unordered_map <std::wstring, import_s> plugins;
    static bool                                        plugins_init = false;

    if (! std::exchange (plugins_init, true))
    {
      auto& sections =
        dll_ini->get_sections ();

      for (auto &[name, section] : sections)
      {
        if (StrStrIW (name.c_str (), L"Import."))
        {
          if (! ( name._Equal (imp_name_reshade) ||
                  name._Equal (imp_name_reshade_ex) ))
          {
            import_s import;

            if (     section.get_value (L"When") == L"Early")
              import.order = SK_Import_LoadOrder::Early;
            else if (section.get_value (L"When") == L"Lazy")
              import.order = SK_Import_LoadOrder::Lazy;
            else
              import.order = SK_Import_LoadOrder::PlugIn;

            wcsncpy_s (import.path,     section.get_value (L"Filename").c_str (), MAX_PATH);
            wcsncpy_s (import.ini_name, name.c_str (),                                  64);

            import.label =
              SK_WideCharToUTF8 (
                wcsrchr (import.ini_name, L'.') + 1
              );

            import.loaded =
              SK_GetModuleHandleW (import.path) != nullptr;

            auto wide_ver_str =
              SK_GetDLLVersionStr (import.path);

            // If DLL has no version, GetDLLVersionStr returns N/A
            if (! wide_ver_str._Equal (L"N/A"))
            {
              import.version =
                SK_WideCharToUTF8 (wide_ver_str);
            }

            plugins.emplace (
              std::make_pair (import.ini_name, import)
            );
          }
        }
      }
    }

    ImGui::Separator ();

    if (ImGui::Button ("Add Plug-In"))
    {
      fileDialog.SetTitle       ("Select a Plug-In DLL");
      fileDialog.SetTypeFilters (  { ".dll", ".asi" }  );
      fileDialog.Open ();
    }

    fileDialog.Display ();

    if (fileDialog.HasSelected ())
    {
      const auto &selections =
        fileDialog.GetMultiSelected ();

      for ( const auto& selected : selections )
      {
        import_s   import;
        wcsncpy_s (import.path, selected.c_str (), MAX_PATH);

        PathStripPathW       (import.path);
        PathRemoveExtensionW (import.path);

        import.label =
          SK_WideCharToUTF8 (import.path);

        wcscat_s  (import.ini_name, 64, import.path);
        wcsncpy_s (import.path,       selected.c_str (), MAX_PATH);

        auto wide_ver_str =
          SK_GetDLLVersionStr (import.path);

        // If DLL has no version, GetDLLVersionStr returns N/A
        if (! wide_ver_str._Equal (L"N/A"))
        {
          import.version =
            SK_WideCharToUTF8 (wide_ver_str);
        }

        plugins.emplace (
          std::make_pair (import.ini_name, import)
        );

        SK_ImGui_SavePlugInPreference ( dll_ini,
                                            import.enabled, import.ini_name,
                             L"ThirdParty", import.order,   import.path );
      }

      fileDialog.ClearSelected ();
    }

    ImGui::SameLine   ();
    ImGui::BeginGroup ();

    for ( auto& [name, import] : plugins )
    {
      ImGui::BeginGroup ();

      bool clicked =
        SK_ImGui_PlugInSelector ( dll_ini,
                                      import.label,    import.path,
                                      import.ini_name, import.enabled,
                                      import.order );

      ImGui::EndGroup ();

      if (ImGui::IsItemHovered () && (! import.version.empty ()))
        ImGui::SetTooltip ( "%hs",      import.version.c_str () );

      if (clicked)
      {
        SK_ImGui_SavePlugInPreference ( dll_ini,
                                          import.enabled, import.ini_name,
                           L"ThirdParty", import.order,   import.path );
      }
    }

    ImGui::EndGroup ();

    ImGui::SameLine   ();
    ImGui::BeginGroup ();

    for ( auto& [name, import] : plugins )
    {
      ImGui::PushID (import.label.c_str ());

      if (! import.loaded)
      {
        if (ImGui::Button ("Load DLL"))
        {
          if (SK_LoadLibraryW (import.path) != nullptr)
          {
            import.loaded = true;
          }
        }
      }

      else
      {
        if (ImGui::Button ("Unload DLL (Risky)"))
        {
          if ( SK_FreeLibrary (
                 SK_GetModuleHandleW (import.path) )
             )
          {
            import.loaded = false;
          }
        }
      }
      ImGui::PopID ();
    }
    ImGui::EndGroup  ();

    SK_ImGui_PlugInDisclaimer ();

    if (changed)
    {
      if (reshade_unofficial)
        reshade_official = false;

      if (reshade_official)
        reshade_unofficial = false;

      SK_ImGui_SavePlugInPreference (dll_ini, reshade_official,   imp_name_reshade,    L"ThirdParty", order,    imp_path_reshade   );
      SK_ImGui_SavePlugInPreference (dll_ini, reshade_unofficial, imp_name_reshade_ex, L"Unofficial", order_ex, imp_path_reshade_ex);

      if (reshade_official)
      {
        // Non-Unity engines benefit from a small (0 ms) injection delay
        if (config.system.global_inject_delay < 0.001)
        {   config.system.global_inject_delay = 0.001;

          if (bUnity)
          {
            SK_ImGui_Warning (
              L"NOTE: This is unlikely to work in a Unity Engine game, a local version of ReShade may be necessary."
            );
          }
        }

        // Remove hook cache records when setting up a plug-in
        dll_ini->get_section (L"DXGI.Hooks").set_name  (L"Invalid.Section.DoNotFlush");
        dll_ini->get_section (L"D3D11.Hooks").set_name (L"Invalid.Section.DoNotFlush");

        dll_ini->write ();
      }
    }

    ImGui::TreePop ();

    return true;
  }

  return false;
}