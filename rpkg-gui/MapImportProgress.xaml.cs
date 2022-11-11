using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Threading;
using System.ComponentModel;
using System.Runtime.InteropServices;
using MahApps.Metro.Controls;
using ControlzEx.Theming;

namespace rpkg
{
    /// <summary>
    /// Interaction logic for MapImportProgress.xaml
    /// </summary>
    public partial class MapImportProgress : MetroWindow
    {
        public MapImportProgress()
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
                this.Top += (sizeInfo.PreviousSize.Height - sizeInfo.NewSize.Height) / 2;

            if (sizeInfo.WidthChanged)
                this.Left += (sizeInfo.PreviousSize.Width - sizeInfo.NewSize.Width) / 2;
        }

        void worker_DoWork(object sender, DoWorkEventArgs e)
        {
            int map_percent_progress_godot_map_nodes = 0;
            int map_percent_progress_map_node_temp_loading = 0;
            int map_percent_progress_map_node_changes_check = 0;
            int map_percent_progress_map_writing_changes_to_qn = 0;
            bool task_done = false;

            while (!task_done)
            {
                task_map_status = get_task_map_status();

                timing_string = Marshal.PtrToStringAnsi(get_timing_string());

                task_status_string = Marshal.PtrToStringAnsi(get_task_status_string());

                if (task_map_status == (int)RPKGStatus.MAP_GODOT_MAP_NODE_IMPORTING)
                {
                    map_percent_progress_godot_map_nodes = get_map_percent_progress_godot_map_nodes();

                    (sender as BackgroundWorker).ReportProgress((int)map_percent_progress_godot_map_nodes);
                }
                else if (task_map_status == (int)RPKGStatus.MAP_NODE_TEMP_LOADING)
                {
                    map_percent_progress_map_node_temp_loading = get_map_percent_progress_map_node_temp_loading();

                    (sender as BackgroundWorker).ReportProgress((int)map_percent_progress_map_node_temp_loading);
                }
                else if (task_map_status == (int)RPKGStatus.MAP_NODE_CHANGES_CHECK)
                {
                    map_percent_progress_map_node_changes_check = get_map_percent_progress_map_node_changes_check();

                    (sender as BackgroundWorker).ReportProgress((int)map_percent_progress_map_node_changes_check);
                }
                else if (task_map_status == (int)RPKGStatus.MAP_WRITING_CHANGES_TO_QN)
                {
                    map_percent_progress_map_writing_changes_to_qn = get_map_percent_progress_map_writing_changes_to_qn();

                    (sender as BackgroundWorker).ReportProgress((int)map_percent_progress_map_writing_changes_to_qn);
                }
                else if (task_map_status == (int)RPKGStatus.MAP_IMPORT_SUCCESSFUL)
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
            if (task_map_status == (int)RPKGStatus.MAP_GODOT_MAP_NODE_IMPORTING)
            {
                ProgressBar1.Value = e.ProgressPercentage;
                ProgressBar2.Value = 0;
                ProgressBar3.Value = 0;
                ProgressBar4.Value = 0;
                message1.Content = timing_string;
            }
            else if (task_map_status == (int)RPKGStatus.MAP_NODE_TEMP_LOADING)
            {
                ProgressBar1.Value = 100;
                ProgressBar2.Value = e.ProgressPercentage;
                ProgressBar3.Value = 0;
                ProgressBar4.Value = 0;
                message1.Content = "Importing map node data from Godot scene file: 100% done";
                message2.Content = timing_string;
            }
            else if (task_map_status == (int)RPKGStatus.MAP_NODE_CHANGES_CHECK)
            {
                ProgressBar1.Value = 100;
                ProgressBar2.Value = 100;
                ProgressBar3.Value = e.ProgressPercentage;
                ProgressBar4.Value = 0;
                message1.Content = "Importing map node data from Godot scene file: 100% done";
                message2.Content = "Loading map node unique entities (TEMP/TBLU): 100% done";
                message3.Content = timing_string;
            }
            else if (task_map_status == (int)RPKGStatus.MAP_WRITING_CHANGES_TO_QN)
            {
                ProgressBar1.Value = 100;
                ProgressBar2.Value = 100;
                ProgressBar3.Value = 100;
                ProgressBar4.Value = e.ProgressPercentage;
                message1.Content = "Importing map node data from Godot scene file: 100% done";
                message2.Content = "Loading map node unique entities (TEMP/TBLU): 100% done";
                message3.Content = "Checking map nodes for changes: 100% done";
                message4.Content = timing_string;
            }
            else
            {
                ProgressBar1.Value = 0;
                ProgressBar2.Value = 0;
                ProgressBar3.Value = 0;
                ProgressBar4.Value = 0;
                message1.Content = "Initiating Map Import...";
            }
        }

        void worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            normal_exit = true;
            this.Close();
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            if (!normal_exit)
            {
                System.Windows.MessageBoxResult response = System.Windows.MessageBox.Show("Are you sure you want to cancel the current operation?", "", System.Windows.MessageBoxButton.YesNo, System.Windows.MessageBoxImage.Question, System.Windows.MessageBoxResult.OK);

                if (response == System.Windows.MessageBoxResult.No)
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

        [DllImport("rpkg-lib.dll", EntryPoint = "get_current_percent", CallingConvention = CallingConvention.Cdecl)]
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

        [DllImport("rpkg-lib.dll", EntryPoint = "get_map_percent_progress_godot_map_nodes", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_map_percent_progress_godot_map_nodes();

        [DllImport("rpkg-lib.dll", EntryPoint = "get_map_percent_progress_map_node_temp_loading", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_map_percent_progress_map_node_temp_loading();

        [DllImport("rpkg-lib.dll", EntryPoint = "get_map_percent_progress_map_node_changes_check", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_map_percent_progress_map_node_changes_check();

        [DllImport("rpkg-lib.dll", EntryPoint = "get_map_percent_progress_map_writing_changes_to_qn", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_map_percent_progress_map_writing_changes_to_qn();
    }
}
