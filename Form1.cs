using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System;
using System.IO;
using System.Net;

namespace Air_Conditioner_Control
{
    public partial class Form1 : Form
    {

        public const int WM_NCLBUTTONDOWN = 0xA1;
        public const int HT_CAPTION = 0x2;
        public static int toggle = 0;
        public static string ipAddress = "";

        [DllImportAttribute("user32.dll")]
        public static extern int SendMessage(IntPtr hWnd,
                         int Msg, int wParam, int lParam);
        [DllImportAttribute("user32.dll")]
        public static extern bool ReleaseCapture();

        public Form1()
        {
            InitializeComponent();
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged_1(object sender, EventArgs e)
        {

        }

        private void incTemp_Click(object sender, EventArgs e)
        {
            WebRequest request = WebRequest.Create("http://" + ipAddress + "/tempInc");
            request.Timeout = 30000;
            WebResponse response = request.GetResponse();
            response.Close();
        }

        private void decTemp_Click(object sender, EventArgs e)
        {
            WebRequest request = WebRequest.Create("http://" + ipAddress + "/tempDec");
            request.Timeout = 30000;
            WebResponse response = request.GetResponse();
            response.Close();
        }

        private void textBox1_TextChanged_2(object sender, EventArgs e)
        {

        }

        private void exit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void minimize_Click(object sender, EventArgs e)
        {
            this.WindowState = FormWindowState.Minimized;
        }

        private void panel1_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                ReleaseCapture();
                SendMessage(Handle, WM_NCLBUTTONDOWN, HT_CAPTION, 0);
            }
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label1_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                ReleaseCapture();
                SendMessage(Handle, WM_NCLBUTTONDOWN, HT_CAPTION, 0);
            }
        }

        private void textBox1_TextChanged_3(object sender, EventArgs e)
        {

        }

        private void power_Click(object sender, EventArgs e)
        {
            WebRequest request;
            WebResponse response;

            if (toggle == 0)
            {
                request = WebRequest.Create("http://" + ipAddress + "/relayOn");
                response = request.GetResponse();
                request.Timeout = 30000;
                response.Close();
                toggle++;
            } else
            {
                request = WebRequest.Create("http://" + ipAddress + "/relayOff");
                response = request.GetResponse();
                request.Timeout = 30000;
                response.Close();
                toggle--;
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            ipAddress = enterIp.Text;
        }
    }
}
