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
    /// Interaction logic for Progress.xaml
    /// </summary>
    public partial class Progress : MetroWindow
    {
        public Progress()
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
            int percent = 0;
            int task_single_status = 0;
            int task_multiple_status = 0;
            bool task_done = false;

            while (!task_done)
            {
                percent = get_current_percent();

                task_single_status = get_task_single_status();

                task_multiple_status = get_task_multiple_status();

                timing_string = Marshal.PtrToStringAnsi(get_timing_string());

                task_status_string = Marshal.PtrToStringAnsi(get_task_status_string());

                (sender as BackgroundWorker).ReportProgress((int)percent);

                if (task_single_status == (int)RPKGStatus.TASK_EXITED)
                {
                    task_status = task_single_status;

                    task_done = true;
                }

                if (operation == (int)Operation.TEMP_TBLU)
                {
                    if (task_multiple_status == (int)RPKGStatus.TASK_SUCCESSFUL)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.TEMP_TBLU_NOT_FOUND_IN_DEPENDS)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.TEMP_TBLU_NOT_FOUND_IN_RPKG)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.TEMP_TBLU_TOO_MANY)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.TEMP_HEADER_NOT_FOUND)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.TEMP_TBLU_ENTRY_COUNT_MISMATCH)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.TEMP_VERSION_UNKNOWN)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.TBLU_VERSION_UNKNOWN)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                }
                else if (operation == (int)Operation.PRIM)
                {
                    if (task_multiple_status == (int)RPKGStatus.TASK_SUCCESSFUL)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_UV_CHANNEL_COUNT_GREATER_THAN_1)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_OBJECT_IS_NOT_A_MESH_TYPE)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                }
                else if (operation == (int)Operation.PRIM_REBUILD || operation == (int)Operation.PRIM_MODEL_REBUILD || operation == (int)Operation.PRIM_MODEL_EXTRACT)
                {
                    if (operation == (int)Operation.PRIM_REBUILD)
                    {
                        if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_SUCCESSFUL)
                        {
                            task_status = task_multiple_status;

                            task_done = true;
                        }
                    }

                    if (operation == (int)Operation.PRIM_MODEL_REBUILD)
                    {
                        if (task_multiple_status == (int)RPKGStatus.PRIM_MODEL_REBUILD_SUCCESSFUL)
                        {
                            task_status = task_multiple_status;

                            task_done = true;
                        }
                    }

                    if (operation == (int)Operation.PRIM_MODEL_EXTRACT)
                    {
                        if (task_multiple_status == (int)RPKGStatus.PRIM_MODEL_EXTRACT_SUCCESSFUL)
                        {
                            task_status = task_multiple_status;

                            task_done = true;
                        }
                    }

                    if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_GLB_MESH_NAME_MALFORMED)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_ONLY_ONE_MESH_ALLOWED)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_VERTEX_NOT_MULTIPLE_OF_3)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_MISSING_POSITION_DATA)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_MISMATCHED_BONES)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_WEIGHTED_DATA_DOES_NOT_CONFORM)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_WEIGHTED_DATA_MISSING)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_NORMALS_DO_NOT_MATCH_VERTICES)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_MISSING_NORMAL_DATA)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_UVS_DO_NOT_MATCH_VERTICES)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_MISSING_UV_DATA)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_COLORS_DO_NOT_MATCH_VERTICES)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_COLORS_WRONG_FORMAT)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                    else if (task_multiple_status == (int)RPKGStatus.PRIM_REBUILD_TOO_MANY_PRIMARY_OBJECT_HEADERS)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                }
                else if (operation == (int)Operation.MASS_EXTRACT)
                {
                    if (task_multiple_status == (int)RPKGStatus.TASK_SUCCESSFUL)
                    {
                        task_status = task_multiple_status;

                        task_done = true;
                    }
                }
                else
                {
                    if (task_single_status == (int)RPKGStatus.TASK_RETURNED)
                    {
                        task_status = task_single_status;

                        task_done = true;
                    }

                    if (task_single_status == (int)RPKGStatus.TASK_SUCCESSFUL)
                    {
                        task_status = task_single_status;

                        task_done = true;
                    }
                }

                Thread.Sleep(100);
            }
        }

        void worker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            ProgressBar.Value = e.ProgressPercentage;
            message.Content = timing_string;
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

        public enum Operation
        {
            IMPORT,
            GENERAL,
            MASS_EXTRACT,
            DOWNLOAD,
            TEMP_TBLU,
            PRIM,
            PRIM_REBUILD,
            PRIM_MODEL_REBUILD,
            PRIM_MODEL_EXTRACT,
            DEEP_SEARCH
        };

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

        public string timing_string = "";
        private bool normal_exit = false;
        public int operation = 0;
        public int task_status = 0;
        public string task_status_string = "";

        [DllImport("rpkg.dll", EntryPoint = "get_current_percent", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_current_percent();

        [DllImport("rpkg.dll", EntryPoint = "get_task_single_status", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_task_single_status();

        [DllImport("rpkg.dll", EntryPoint = "reset_task_single_status", CallingConvention = CallingConvention.Cdecl)]
        public static extern int reset_task_single_status();

        [DllImport("rpkg.dll", EntryPoint = "get_task_multiple_status", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_task_multiple_status();

        [DllImport("rpkg.dll", EntryPoint = "reset_task_multiple_status", CallingConvention = CallingConvention.Cdecl)]
        public static extern int reset_task_multiple_status();

        [DllImport("rpkg.dll", EntryPoint = "set_gui_control", CallingConvention = CallingConvention.Cdecl)]
        public static extern int set_gui_control(int gui_control_value);

        [DllImport("rpkg.dll", EntryPoint = "get_gui_control", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_gui_control();

        [DllImport("rpkg.dll", EntryPoint = "get_timing_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_timing_string();

        [DllImport("rpkg.dll", EntryPoint = "get_task_status_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_task_status_string();
    }
}
