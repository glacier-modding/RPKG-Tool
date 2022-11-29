using System;
using System.Windows;
using System.Threading;
using System.ComponentModel;
using System.Runtime.InteropServices;
using MahApps.Metro.Controls;

namespace rpkg
{
    /// <summary>
    /// Interaction logic for MapExportProgress.xaml
    /// </summary>
    public partial class MapExportProgress : MetroWindow
    {
        public MapExportProgress()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            BackgroundWorker worker = new BackgroundWorker();
            worker.WorkerReportsProgress = true;
            worker.DoWork += worker_DoWork;
            worker.ProgressChanged += worker_ProgressChanged;
            worker.RunWorkerCompleted += worker_RunWorkerCompleted;

            worker.RunWorkerAsync();
        }
        protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        {
            base.OnRenderSizeChanged(sizeInfo);

            if (sizeInfo.HeightChanged)
                Top += (sizeInfo.PreviousSize.Height - sizeInfo.NewSize.Height) / 2;

            if (sizeInfo.WidthChanged)
                Left += (sizeInfo.PreviousSize.Width - sizeInfo.NewSize.Width) / 2;
        }

        void worker_DoWork(object sender, DoWorkEventArgs e)
        {
            int map_percent_progress_recursive_temp = 0;
            int map_percent_progress_map_nodes = 0;
            int map_percent_progress_map_nodes_prim = 0;
            int map_percent_progress_godot_files = 0;
            bool task_done = false;

            while (!task_done)
            {
                task_map_status = get_task_map_status();

                timing_string = Marshal.PtrToStringAnsi(get_timing_string());

                task_status_string = Marshal.PtrToStringAnsi(get_task_status_string());

                if (task_map_status == (int)RPKGStatus.MAP_RECURSIVE_TEMP_LOADING_EXECUTING)
                {
                    map_percent_progress_recursive_temp = get_map_percent_progress_recursive_temp();

                    (sender as BackgroundWorker).ReportProgress((int)map_percent_progress_recursive_temp);
                }
                else if (task_map_status == (int)RPKGStatus.MAP_GET_MAP_NODES_EXECUTING)
                {
                    map_percent_progress_map_nodes = get_map_percent_progress_map_nodes();

                    (sender as BackgroundWorker).ReportProgress((int)map_percent_progress_map_nodes);
                }
                else if (task_map_status == (int)RPKGStatus.MAP_EXTRACT_MAP_NODES_PRIMS_EXECUTING)
                {
                    map_percent_progress_map_nodes_prim = get_map_percent_progress_map_nodes_prim();

                    (sender as BackgroundWorker).ReportProgress((int)map_percent_progress_map_nodes_prim);
                }
                else if (task_map_status == (int)RPKGStatus.MAP_GENERATE_GODOT_PROJECT_EXECUTING)
                {
                    map_percent_progress_godot_files = get_map_percent_progress_godot_files();

                    (sender as BackgroundWorker).ReportProgress((int)map_percent_progress_godot_files);
                }
                else if (task_map_status == (int)RPKGStatus.MAP_EXPORT_SUCCESSFUL)
                {
                    task_status = task_map_status;

                    task_done = true;
                }
                else if (task_map_status == (int)RPKGStatus.MAP_ERROR)
                {
                    task_status = task_map_status;

                    task_status_string = Marshal.PtrToStringAnsi(get_task_status_string());

                    task_done = true;
                }

                Thread.Sleep(100);
            }
        }

        void worker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            if (task_map_status == (int)RPKGStatus.MAP_RECURSIVE_TEMP_LOADING_EXECUTING)
            {
                ProgressBar1.Value = e.ProgressPercentage;
                ProgressBar2.Value = 0;
                ProgressBar3.Value = 0;
                ProgressBar4.Value = 0;
                message1.Content = timing_string;
            }
            else if (task_map_status == (int)RPKGStatus.MAP_GET_MAP_NODES_EXECUTING)
            {
                ProgressBar1.Value = 100;
                ProgressBar2.Value = e.ProgressPercentage;
                ProgressBar3.Value = 0;
                ProgressBar4.Value = 0;
                message1.Content = "Loading recursive TEMP/TBLU entities: 100% done";
                message2.Content = timing_string;
            }
            else if (task_map_status == (int)RPKGStatus.MAP_EXTRACT_MAP_NODES_PRIMS_EXECUTING)
            {
                ProgressBar1.Value = 100;
                ProgressBar2.Value = 100;
                ProgressBar3.Value = e.ProgressPercentage;
                ProgressBar4.Value = 0;
                message1.Content = "Loading recursive TEMP/TBLU entities: 100% done";
                message2.Content = "Loading map nodes: 100% done";
                message3.Content = timing_string;
            }
            else if (task_map_status == (int)RPKGStatus.MAP_GENERATE_GODOT_PROJECT_EXECUTING)
            {
                ProgressBar1.Value = 100;
                ProgressBar2.Value = 100;
                ProgressBar3.Value = 100;
                ProgressBar4.Value = e.ProgressPercentage;
                message1.Content = "Loading recursive TEMP/TBLU entities: 100% done";
                message2.Content = "Loading map nodes: 100% done";
                message3.Content = "Extracting all map's PRIMs as GLBs: 100% done";
                message4.Content = timing_string;
            }
            else
            {
                ProgressBar1.Value = 0;
                ProgressBar2.Value = 0;
                ProgressBar3.Value = 0;
                ProgressBar4.Value = 0;
                message1.Content = "Initiating Map Export...";
            }
        }

        void worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            normal_exit = true;
            Close();
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            if (!normal_exit)
            {
                MessageBoxResult response = MessageBox.Show("Are you sure you want to cancel the current operation?", "", MessageBoxButton.YesNo, MessageBoxImage.Question, MessageBoxResult.OK);

                if (response == MessageBoxResult.No)
                {
                    e.Cancel = true;
                }
                else
                {
                    int gui_control_value = (int)RPKGStatus.ABORT_CURRENT_TASK;

                    int return_value = set_gui_control(gui_control_value);
                }
            }
        }

        public enum RPKGStatus
        {
            READY,
            TASK_EXECUTING,
            TASK_SUCCESSFUL,
            TASK_EXITED,
            TASK_RETURNED,
            RPKG_IMPORTING,
            RPKG_IMPORTED,
            RPKG_ALREADY_IMPORTED,
            SCANNING_DIRECTORY,
            SCANNING_DIRECTORY_DONE,
            ABORT_CURRENT_TASK,
            TEMP_TBLU_FOUND,
            TEMP_TBLU_NOT_FOUND_IN_DEPENDS,
            TEMP_TBLU_NOT_FOUND_IN_RPKG,
            TEMP_TBLU_TOO_MANY,
            TEMP_HEADER_NOT_FOUND,
            TEMP_TBLU_ENTRY_COUNT_MISMATCH,
            PRIM_UV_CHANNEL_COUNT_GREATER_THAN_1,
            PRIM_OBJECT_IS_NOT_A_MESH_TYPE,
            TEMP_VERSION_UNKNOWN,
            TBLU_VERSION_UNKNOWN,
            PRIM_REBUILD_GLB_MESH_NAME_MALFORMED,
            PRIM_REBUILD_ONLY_ONE_MESH_ALLOWED,
            PRIM_REBUILD_VERTEX_NOT_MULTIPLE_OF_3,
            PRIM_REBUILD_MISSING_POSITION_DATA,
            PRIM_REBUILD_MISMATCHED_BONES,
            PRIM_REBUILD_WEIGHTED_DATA_DOES_NOT_CONFORM,
            PRIM_REBUILD_WEIGHTED_DATA_MISSING,
            PRIM_REBUILD_NORMALS_DO_NOT_MATCH_VERTICES,
            PRIM_REBUILD_MISSING_NORMAL_DATA,
            PRIM_REBUILD_UVS_DO_NOT_MATCH_VERTICES,
            PRIM_REBUILD_MISSING_UV_DATA,
            PRIM_REBUILD_COLORS_DO_NOT_MATCH_VERTICES,
            PRIM_REBUILD_COLORS_WRONG_FORMAT,
            PRIM_REBUILD_TOO_MANY_PRIMARY_OBJECT_HEADERS,
            PRIM_REBUILD_META_FILE_MISSING,
            PRIM_REBUILD_SUCCESSFUL,
            PRIM_MODEL_REBUILD_SUCCESSFUL,
            PRIM_MODEL_EXTRACT_SUCCESSFUL,
            MAP_RECURSIVE_TEMP_LOADING_EXECUTING,
            MAP_GET_MAP_NODES_EXECUTING,
            MAP_EXTRACT_MAP_NODES_PRIMS_EXECUTING,
            MAP_GENERATE_GODOT_PROJECT_EXECUTING,
            MAP_EXPORT_SUCCESSFUL,
            MAP_GODOT_MAP_NODE_IMPORTING,
            MAP_NODE_TEMP_LOADING,
            MAP_NODE_CHANGES_CHECK,
            MAP_WRITING_CHANGES_TO_QN,
            MAP_IMPORT_SUCCESSFUL,
            MAP_ERROR
        };

        int task_map_status = 0;
        public string timing_string = "";
        private bool normal_exit = false;
        public int operation = 0;
        public int task_status = 0;
        public string task_status_string = "";

        [DllImport("rpkg-dll.dll", EntryPoint = "get_current_percent", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_current_percent();

        [DllImport("rpkg-lib.dll", EntryPoint = "get_task_map_status", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_task_map_status();

        [DllImport("rpkg-lib.dll", EntryPoint = "reset_task_map_status", CallingConvention = CallingConvention.Cdecl)]
        public static extern int reset_task_map_status();

        [DllImport("rpkg-lib.dll", EntryPoint = "set_gui_control", CallingConvention = CallingConvention.Cdecl)]
        public static extern int set_gui_control(int gui_control_value);

        [DllImport("rpkg-lib.dll", EntryPoint = "get_gui_control", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_gui_control();

        [DllImport("rpkg-lib.dll", EntryPoint = "get_timing_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_timing_string();

        [DllImport("rpkg-lib.dll", EntryPoint = "get_task_status_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_task_status_string();

        [DllImport("rpkg-lib.dll", EntryPoint = "get_map_percent_progress_recursive_temp", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_map_percent_progress_recursive_temp();

        [DllImport("rpkg-lib.dll", EntryPoint = "get_map_percent_progress_map_nodes", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_map_percent_progress_map_nodes();

        [DllImport("rpkg-lib.dll", EntryPoint = "get_map_percent_progress_map_nodes_prim", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_map_percent_progress_map_nodes_prim();

        [DllImport("rpkg-lib.dll", EntryPoint = "get_map_percent_progress_godot_files", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_map_percent_progress_godot_files();
    }
}
