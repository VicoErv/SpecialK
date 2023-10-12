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

#include <implot/implot.h>

extern iSK_INI* osd_ini;

#include <SpecialK/nvapi.h>
#include <SpecialK/render/d3d11/d3d11_core.h>

namespace ImPlot {

template <typename T>
inline T RandomRange(T min, T max) {
    T scale = rand() / (T) RAND_MAX;
    return min + scale * ( max - min );
}

}

void
SK_ImGui_DrawGraph_Latency ()
{
  if (! sk::NVAPI::nv_hardware) {
    return;
  }

  static auto& rb =
    SK_GetCurrentRenderBackend ();

  if (! rb.isReflexSupported ())
    return;

  ImGui::BeginGroup ();

  NV_LATENCY_RESULT_PARAMS
    latencyResults         = {                          };
    latencyResults.version = NV_LATENCY_RESULT_PARAMS_VER;

  struct stage_timing_s
  {
    const char* label = nullptr;
    const char*
           desc = nullptr;
    NvU64   min = std::numeric_limits <NvU64>::max (),
            max = std::numeric_limits <NvU64>::min (),
            sum = 0;
    double  avg = 0.0;
    int samples = 0;
    NvU64 start = 0;
    NvU64   end = 0;
    ImColor color;

    double durations [64] = { };

    void reset (void)
    {
      min     = std::numeric_limits <NvU64>::max (),
      max     = std::numeric_limits <NvU64>::min (),
      sum     = 0;
      avg     = 0.0;
      samples = 0;
      start   = 0;
      end     = 0;

      memset (durations, 0, sizeof (double) * 64);
    }
  };

  struct frame_timing_s {
    NvU32 gpuActiveRenderTimeUs;
    NvU32 gpuFrameTimeUs;
  } gpu_frame_times [64];
  
  static stage_timing_s sim      { "Simulation"       }; static stage_timing_s render   { "Render Submit"   };
  static stage_timing_s specialk { "Special K"        }; static stage_timing_s present  { "Present"         };
  static stage_timing_s driver   { "Driver"           }; static stage_timing_s os       { "OS Render Queue" };
  static stage_timing_s gpu      { "GPU Render"       }; static stage_timing_s gpu_busy { "GPU Busy"        };
  static stage_timing_s total    { "Total Frame Time" };
  static stage_timing_s input    { "Input Age"        };

  total.reset ();
  input.reset ();

  stage_timing_s* stages [] = {
    &sim,     &render, &specialk,
    &present, &driver, &os,
    &gpu
  };

  specialk.desc = "Includes drawing SK's overlay and framerate limiting";

  float fLegendY = 0.0f; // Anchor pt to align latency timing legend w/ text
  int   id       = 0;

  for ( auto* stage : stages )
  {
    stage->color =
      ImColor::HSV ( ( ++id ) /
        static_cast <float> (
             sizeof ( stages) /
             sizeof (*stages) ), 0.5f,
                                 1.0f );

    stage->reset ();
  }

  if (rb.getLatencyReportNV (&latencyResults))
  {
    for ( auto idx  = 63 ;
               idx >= 0  ;
             --idx )
    {
      auto& frame =
        latencyResults.frameReport
              [idx];

      gpu_frame_times [idx].gpuActiveRenderTimeUs = frame.gpuActiveRenderTimeUs;
      gpu_frame_times [idx].gpuFrameTimeUs        = frame.gpuFrameTimeUs;

      auto _UpdateStat =
      [&]( NvU64           start,
           NvU64           end,
           stage_timing_s* stage )
      {
        if (start != 0 && end != 0)
        {
          auto
              duration =
                 (
             end - start
                 );
          if (duration >= 0 && duration < 666666)
          {
            stage->samples++;

            stage->durations [idx] = static_cast <double> (duration) / 1000.0;

            stage->sum += duration;
            stage->min = static_cast <NvU64>
                       ( (duration < stage->min) ?
                          duration : stage->min );
            stage->max = static_cast <NvU64>
                       ( (duration > stage->max) ?
                          duration : stage->max );

            if (idx == 63)
            {
              stage->start = start;
              stage->end   = end;
            }
          }
        }
      };

      _UpdateStat (frame.simStartTime,           frame.simEndTime,            &sim);
      _UpdateStat (frame.renderSubmitStartTime,  frame.renderSubmitEndTime,   &render);
      _UpdateStat (frame.presentStartTime,       frame.presentEndTime,        &present);
      _UpdateStat (frame.driverStartTime,        frame.driverEndTime,         &driver);
      _UpdateStat (frame.osRenderQueueStartTime, frame.osRenderQueueEndTime,  &os);
      _UpdateStat (frame.gpuRenderStartTime,     frame.gpuRenderEndTime,      &gpu);
      _UpdateStat (frame.simStartTime,           frame.gpuRenderEndTime,      &total);
      _UpdateStat (frame.inputSampleTime,        frame.gpuRenderEndTime,      &input);
      _UpdateStat (frame.renderSubmitEndTime,    frame.presentStartTime,      &specialk);
    }

    auto _UpdateAverages = [&](stage_timing_s* stage)
    {
      if (stage->samples != 0)
      {
        stage->avg = static_cast <double> (stage->sum) /
                     static_cast <double> (stage->samples);
      }
    };

    stage_timing_s *min_stage = &sim;
    stage_timing_s *max_stage = &sim;

    for (auto* pStage : stages)
    {
      _UpdateAverages (    pStage     );

      if (min_stage->avg > pStage->avg) {
          min_stage      = pStage;
      }

      if (max_stage->avg < pStage->avg) {
          max_stage      = pStage;
      }
    }

    gpu_busy.start = latencyResults.frameReport [63].gpuRenderEndTime - gpu_frame_times [63].gpuActiveRenderTimeUs;
    gpu_busy.end   = latencyResults.frameReport [63].gpuRenderEndTime;
    gpu_busy.avg   = static_cast <double> (gpu_busy.end - gpu_busy.start);

    _UpdateAverages (&input);
    _UpdateAverages (&total);

    if (input.avg > total.avg) {
        input.avg = 0.0;
    }

    static double xs1[64], ys1[64],
                  ys2[64], ys3[64], ys4[64], ys5[64];

    double dCPU = 0.0;
    double dGPU = 0.0;

    for (int i = 0; i < 64; ++i)
    {
      xs1[i] = (float)i;
      ys1[i] = sim.durations    [i];
      ys2[i] = render.durations [i];
      ys3[i] = gpu.durations    [i];
      ys4[i] = static_cast <double> (gpu_frame_times [i].gpuActiveRenderTimeUs) / 1000.0;
      ys5[i] = ys1[i]+ys2[i]; // Start of GPU-only workload

      dCPU += ys5 [i];
      dGPU += ys4 [i];
    }

    dCPU /= 64.0;
    dGPU /= 64.0;

    static bool show_lines = true;
    static bool show_fills = true;


    if (ImPlot::BeginPlot ("##Stage Time", ImVec2 (-1, 0), ImPlotFlags_NoTitle | ImPlotFlags_NoInputs | ImPlotFlags_NoMouseText
                                                                               | ImPlotFlags_NoMenus  | ImPlotFlags_NoBoxSelect))
    {
      auto flags =
        ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_AutoFit;
      
      ImPlot::SetupAxes       ("Frame", "Milliseconds", flags | ImPlotAxisFlags_NoLabel |
                                                                ImPlotAxisFlags_NoTickLabels, flags/*& ~ImPlotAxisFlags_AutoFit*/);
      //ImPlot::SetupAxesLimits (0, 63, 0, 10000.0 * total.avg / static_cast <double> (SK_QpcFreq), ImPlotCond_Always);
      ImPlot::SetupLegend     (ImPlotLocation_SouthWest, ImPlotLegendFlags_Horizontal);

      if (show_lines)
      {
        ImPlot::PlotLine ("Simulation",    xs1, ys1, 64);
        ImPlot::PlotLine ("Render Submit", xs1, ys2, 64);
        ImPlot::PlotLine ("GPU Scheduled", xs1, ys3, 64);
        ImPlot::PlotLine ("GPU Busy",      xs1, ys4, 64);
      }

      if (show_fills)
      {
        ImPlot::DragLineY (0,&dGPU,ImVec4(1,0,0,1),1,ImPlotDragToolFlags_NoFit);
        ImPlot::DragLineY (0,&dCPU,ImVec4(0,0,1,1),1,ImPlotDragToolFlags_NoFit);

        if (dGPU > dCPU * 1.25f)
          ImPlot::TagY (dGPU, ImVec4 (1,0,0,1), "GPU Bound");
        else if (dCPU > dGPU * 1.25f)
          ImPlot::TagY (dCPU, ImVec4 (0,0,1,1), "CPU Bound");

        ImPlot::PushStyleVar (ImPlotStyleVar_FillAlpha, 0.1f);
      //ImPlot::PlotShaded   ("Simulation",    xs1, ys1, 64, -INFINITY, flags);
      //ImPlot::PlotShaded   ("Render Submit", xs1, ys2, 64, -INFINITY, flags);
      //ImPlot::PlotShaded   ("GPU Scheduled", xs1, ys3, 64, -INFINITY, flags);
      //ImPlot::PlotShaded   ("GPU Busy",      xs1, ys4, 64, -INFINITY, flags);
        ImPlot::PlotShaded   ("GPU Busy",      xs1, ys4, ys5, 64, flags);
        ImPlot::PlotShaded   ("CPU Busy",      xs1, ys5, 64, -INFINITY, flags);
      //ImPlot::PlotShaded   ("GPU Idle",      xs1, ys3, ys4, 64, flags);
        ImPlot::PopStyleVar  ();
      }
      ImPlot::EndPlot ();
    }

    ImGui::BeginGroup ();
    for ( auto* pStage : stages )
    {
      ImGui::TextColored (
        pStage == min_stage ? ImColor (0.1f, 1.0f, 0.1f) :
        pStage == max_stage ? ImColor (1.0f, 0.1f, 0.1f) :
                              ImColor (.75f, .75f, .75f),
          "%s", pStage->label );

      if (ImGui::IsItemHovered ())
      {
        if (pStage->desc != nullptr) {
          ImGui::SetTooltip ("%s", pStage->desc);
        }
      }
    }
    ImGui::Separator   ();
    ImGui::TextColored (
      ImColor (1.f, 1.f, 1.f), "Total Frame Time"
    );
    if (input.avg != 0.0) {
    ImGui::TextColored (
      ImColor (1.f, 1.f, 1.f), "Input Age"
    );
    }
    ImGui::EndGroup    ();
    ImGui::SameLine    ();
    ImGui::SeparatorEx (ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine    (0.0f, 10.0f);
    ImGui::BeginGroup  ();
    ////for (auto* pStage : stages)
    ////{
    ////  ImGui::TextColored (
    ////    ImColor (0.825f, 0.825f, 0.825f),
    ////      "Min / Max / Avg" );
    ////}
    ////ImGui::EndGroup   ();
    ////ImGui::SameLine   (0.0f);
    ////ImGui::BeginGroup ();
    ////for (auto* pStage : stages)
    ////{
    ////  ImGui::TextColored (
    ////    ImColor (0.905f, 0.905f, 0.905f),
    ////      "%llu", pStage->min );
    ////}
    ////ImGui::EndGroup   ();
    ////ImGui::SameLine   (0.0f);
    ////ImGui::BeginGroup ();
    ////for (auto* pStage : stages)
    ////{
    ////  ImGui::TextColored (
    ////    ImColor (0.915f, 0.915f, 0.915f),
    ////      "%llu", pStage->max );
    ////}
    ////ImGui::EndGroup   ();
    ////ImGui::SameLine   (0.0f);
    ////ImGui::BeginGroup ();
    for (auto* pStage : stages)
    {
      ImGui::TextColored (
        pStage == min_stage ? ImColor (0.6f, 1.0f, 0.6f) :
        pStage == max_stage ? ImColor (1.0f, 0.6f, 0.6f) :
                              ImColor (0.9f, 0.9f, 0.9f),
          "%4.2f ms",
            pStage->avg / 1000.0
      );
    }
    ImGui::Separator   ();

    double frametime = total.avg / 1000.0;

    fLegendY =
      ImGui::GetCursorScreenPos ().y;

    ImGui::TextColored (
      ImColor (1.f, 1.f, 1.f),
        "%4.2f ms",
          frametime    );

    static bool
        input_sampled = false;
    if (input_sampled || input.avg != 0.0)
    {   input_sampled = true;
      ImGui::TextColored (
        ImColor (1.f, 1.f, 1.f),
          input.avg != 0.0 ?
                "%4.2f ms" : "--",
              input.avg / 1000.0
                         );

      fLegendY +=
        ImGui::GetTextLineHeightWithSpacing () / 2.0f;
    }
    ImGui::EndGroup    ();

    ImGui::SameLine    ();
    ImGui::SeparatorEx (ImGuiSeparatorFlags_Vertical);
  }

  ImGui::SameLine   (0, 10.0f);
  ImGui::BeginGroup ();

  extern void
  SK_NV_AdaptiveSyncControl ();
  SK_NV_AdaptiveSyncControl ();


  float fMaxWidth  = ImGui::GetContentRegionAvail        ().x;
  float fMaxHeight = ImGui::GetTextLineHeightWithSpacing () * 2.8f;
  float fInset     = fMaxWidth  *  0.025f;
  float X0         = ImGui::GetCursorScreenPos ().x;
  float Y0         = ImGui::GetCursorScreenPos ().y - ImGui::GetTextLineHeightWithSpacing ();
        fMaxWidth *= 0.95f;

  static DWORD                                  dwLastUpdate = 0;
  static float                                  scale        = 1.0f;
  static std::vector <stage_timing_s>           sorted_stages;
  static _NV_LATENCY_RESULT_PARAMS::FrameReport frame_report;

  if (dwLastUpdate < SK::ControlPanel::current_time - 266)
  {   dwLastUpdate = SK::ControlPanel::current_time;

    frame_report =
      latencyResults.frameReport [63];

    sorted_stages.clear ();

    for (auto* stage : stages)
    {
      sorted_stages.push_back (*stage);
    }

    scale = fMaxWidth /
      static_cast <float> ( frame_report.gpuRenderEndTime -
                            frame_report.simStartTime );

    std::sort ( std::begin (sorted_stages),
                std::end   (sorted_stages),
                [](stage_timing_s& a, stage_timing_s& b)
                {
                  return ( a.start < b.start );
                }
              );
  }

  ImDrawList* draw_list =
    ImGui::GetWindowDrawList ();

 ///draw_list->PushClipRect ( //ImVec2 (0.0f, 0.0f), ImVec2 (ImGui::GetIO ().DisplaySize.x, ImGui::GetIO ().DisplaySize.y) );
 ///                          ImVec2 (X0,                                     Y0),
 ///                          ImVec2 (X0 + ImGui::GetContentRegionAvail ().x, Y0 + fMaxHeight) );

  float x  = X0 + fInset;
  float y  = Y0;
  float dY = fMaxHeight / 7.0f;

  for ( auto& kStage : sorted_stages )
  {
    x = X0 + fInset +
    ( kStage.start - frame_report.simStartTime )
                   * scale;

    float duration =
      (kStage.end - kStage.start) * scale;

    ImVec2 r0 (x,            y);
    ImVec2 r1 (x + duration, y + dY);

    draw_list->AddRectFilled ( r0, r1, kStage.color );

    if (ImGui::IsMouseHoveringRect (r0, r1))
    {
      ImGui::SetTooltip ("%s", kStage.label);
    }

    y += dY;
  }

  if (frame_report.inputSampleTime != 0)
  {
    x = X0 + fInset + ( frame_report.inputSampleTime -
                        frame_report.simStartTime )  *  scale;

    draw_list->AddCircle (
      ImVec2  (x, Y0 + fMaxHeight / 2.0f), 4,
      ImColor (1.0f, 1.0f, 1.0f, 1.0f),    12, 2
    );
  }

 ///draw_list->PopClipRect ();

  ImGui::SetCursorScreenPos (
    ImVec2 (ImGui::GetCursorScreenPos ().x, fLegendY)
  );

  for ( auto *pStage : stages )
  {
    ImGui::TextColored (pStage->color, "%s", pStage->label);

    if (ImGui::IsItemHovered ())
    {
      if (pStage->desc != nullptr) {
        ImGui::SetTooltip ("%s", pStage->desc);
      }
    }

    ImGui::SameLine    (0.0f, 12.0f);
  }

  ImGui::Spacing    ();

  ImGui::EndGroup   ();

  if ( ReadULong64Acquire (&SK_Reflex_LastFrameMarked) <
       ReadULong64Acquire (&SK_RenderBackend::frames_drawn) - 1 )
  {
    static bool bRTSS64 =
      SK_GetModuleHandle (L"RTSSHooks64.dll") != 0;

    ImGui::TextColored ( ImColor::HSV (0.1f, 1.f, 1.f),
                           bRTSS64 ? "RivaTuner Statistics Server detected, "
                                     "latency measurements may be inaccurate."
                                   : "No draw calls captured, "
                                     "latency measurements may be inaccurate." );
  }

  ImGui::EndGroup   ();
}

void
SK_ImGui_DrawConfig_Latency ()
{
  static auto& rb =
    SK_GetCurrentRenderBackend ();

  bool bFullReflexSupport =
    rb.isReflexSupported ();

  if (! (sk::NVAPI::nv_hardware && SK_API_IsDXGIBased (rb.api)))
    return;

  ImGui::BeginGroup ();

  int reflex_mode = 0;

  if (config.nvidia.reflex.enable)
  {
    if (       config.nvidia.reflex.low_latency_boost)
    { if (     config.nvidia.reflex.low_latency)
      reflex_mode = 2;
      else
      reflex_mode = 3;
    } else if (config.nvidia.reflex.low_latency) {
      reflex_mode = 1;
    } else {
      reflex_mode = 0;
    }
  }

  else {
    reflex_mode = 0;
  }

  bool show_mode_select = true;

  if (config.nvidia.reflex.native)
  {
    ImGui::Bullet   ();
    ImGui::SameLine ();
    ImGui::TextColored ( ImColor::HSV (0.1f, 1.f, 1.f),
                           "Game is using native Reflex, data shown here may not update every frame." );

    ImGui::Checkbox ( "Override Game's Reflex Mode",
                        &config.nvidia.reflex.override );

    if (ImGui::IsItemHovered ())
    {
      ImGui::BeginTooltip    ();
      ImGui::TextUnformatted ("This may allow you to reduce latency even more in native Reflex games");
      ImGui::Separator       ();
      ImGui::BulletText      ("Minimum Latency: Low Latency + Boost w/ Latency Marker Trained Optimization");
      ImGui::EndTooltip      ();
    }

    show_mode_select = config.nvidia.reflex.override;
  }

  if (show_mode_select)
  {
    // We can actually use "Nothing But Boost" in
    //   situations where Reflex doesn't normally work
    //     such as DXGI/Vulkan Interop SwapChains.
    if (! bFullReflexSupport) reflex_mode =
                              reflex_mode == 0 ? 0
                                               : 1;

    bool selected =
      rb.isReflexSupported () ?
        ImGui::Combo ( "NVIDIA Reflex Mode", &reflex_mode,
                          "Off\0Low Latency\0"
                               "Low Latency + Boost\0"
                                 "Nothing But Boost\0\0" )
                              :
        ImGui::Combo ( "NVIDIA Reflex Mode", &reflex_mode,
                          "Off\0Nothing But Boost\0\0" );

    if (selected)
    {
      // We can actually use "Nothing But Boost" in
      //   situations where Reflex doesn't normally work
      //     such as DXGI/Vulkan Interop SwapChains.
      if (! bFullReflexSupport) reflex_mode =
                                reflex_mode == 0 ? 0
                                                 : 2;

      switch (reflex_mode)
      {
        case 0:
          config.nvidia.reflex.enable            = false;
          config.nvidia.reflex.low_latency       = false;
          config.nvidia.reflex.low_latency_boost = false;
          break;

        case 1:
          config.nvidia.reflex.enable            = true;
          config.nvidia.reflex.low_latency       = true;
          config.nvidia.reflex.low_latency_boost = false;
          break;

        case 2:
          config.nvidia.reflex.enable            = true;
          config.nvidia.reflex.low_latency       = true;
          config.nvidia.reflex.low_latency_boost = true;
          break;

        case 3:
          config.nvidia.reflex.enable            =  true;
          config.nvidia.reflex.low_latency       = false;
          config.nvidia.reflex.low_latency_boost =  true;
          break;
      }

      rb.driverSleepNV (config.nvidia.reflex.enforcement_site);
    }

    if (ImGui::IsItemHovered ())
      ImGui::SetTooltip ("NOTE: Reflex has greatest impact on G-Sync users -- it may lower peak framerate to minimize latency.");
  }

  if (config.nvidia.reflex.enable && config.nvidia.reflex.low_latency && (! config.nvidia.reflex.native) && bFullReflexSupport)
  {
    config.nvidia.reflex.enforcement_site =
      std::clamp (config.nvidia.reflex.enforcement_site, 0, 1);

    ImGui::Combo ( "NVIDIA Reflex Trigger Point", &config.nvidia.reflex.enforcement_site,
                     "End-of-Frame\0Start-of-Frame\0" );

  //bool unlimited =
  //  config.nvidia.reflex.frame_interval_us == 0;
  //
  //if (ImGui::Checkbox ("Use Unlimited Reflex FPS", &unlimited))
  //{
  //  extern float __target_fps;
  //
  //  if (unlimited) config.nvidia.reflex.frame_interval_us = 0;
  //  else           config.nvidia.reflex.frame_interval_us =
  //           static_cast <UINT> ((1000.0 / __target_fps) * 1000.0);
  //}
  }
  
  if ( config.nvidia.reflex.enable            &&
       config.nvidia.reflex.low_latency       &&
       config.nvidia.reflex.low_latency_boost && ((! config.nvidia.reflex.native) || config.nvidia.reflex.override)
                                              && rb.isReflexSupported () )
  {
    ImGui::Checkbox ("Use Latency Marker Trained Optimization", &config.nvidia.reflex.marker_optimization);

    if (ImGui::IsItemHovered ())
    {
      ImGui::SetTooltip (
        "Uses timing data collected by SK (i.e. game's first draw call) to aggressively reduce latency"
        " at the expense of frame pacing."
      );
    }
  }
  ImGui::EndGroup   ();
  ImGui::Separator  ();
}

class SKWG_Latency : public SK_Widget
{
public:
  SKWG_Latency () noexcept : SK_Widget ("Latency Analysis")
  {
    SK_ImGui_Widgets->latency = this;

    setResizable    (                false).setAutoFit (true).setMovable (false).
    setDockingPoint (DockAnchor::SouthWest).setVisible (false);
  };

  void load (iSK_INI* cfg) noexcept override
  {
    SK_Widget::load (cfg);
  }

  void save (iSK_INI* cfg) noexcept override
  {
    if (cfg == nullptr) {
      return;
    }

    SK_Widget::save (cfg);

    cfg->write ();
  }

  void run () noexcept override
  {
    static auto* cp =
      SK_GetCommandProcessor ();

    static volatile LONG init = 0;

    if (! InterlockedCompareExchange (&init, 1, 0))
    {
      const float ui_scale = ImGui::GetIO ().FontGlobalScale;

      setMinSize (ImVec2 (50.0f * ui_scale, 50.0f * ui_scale));
    }

    if (ImGui::GetFont () == nullptr) {
      return;
    }
  }

  void draw () override
  {
    if (ImGui::GetFont () == nullptr) {
      return;
    }

    // Prevent showing this widget if conditions are not met
    if (! SK_GetCurrentRenderBackend ().isReflexSupported ())
      setVisible (false);

    static const auto& io =
      ImGui::GetIO ();

    static bool move = true;

    if (move)
    {
      ImGui::SetWindowPos (
        ImVec2 ( io.DisplaySize.x - getSize ().x,
                 io.DisplaySize.y - getSize ().y )
      );

      move = false;
    }

    SK_ImGui_DrawGraph_Latency ();
  }


  void config_base () noexcept override
  {
    SK_Widget::config_base ();

    ImGui::Separator (  );
    ImGui::TreePush  ("");

    SK_ImGui_DrawConfig_Latency ();

    ImGui::TreePop   (  );
  }

  void OnConfig (ConfigEvent event) noexcept override
  {
    switch (event)
    {
      case SK_Widget::ConfigEvent::LoadComplete:
        break;

      case SK_Widget::ConfigEvent::SaveStart:
        break;
    }
  }
};

SK_LazyGlobal <SKWG_Latency> __latency__;

void SK_Widget_InitLatency ()
{
  SK_RunOnce (__latency__.get ());
}