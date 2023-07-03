using Microsoft.Data.SqlClient;
using System.Data;

namespace TNTools
{
    public partial class frmMain : Form
    {
        public frmMain()
        {
            InitializeComponent();
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            
        }

        private void btnCreateGcpAndGln_Click(object sender, EventArgs e)
        {
            try
            {
                if (string.IsNullOrEmpty(txtUsername.Text))
                {
                    MessageBox.Show("Điền cái tên đăng nhập vào");
                    return;
                }

                var mesg = SQLServerClient.GenerateGLNAndGCPCode(txtUsername.Text, (int)numGLNCode.Value, (int)numGCPCode.Value);
                ShowInfomation(mesg);
            }
            catch (Exception ex)
            {
                ShowInfomation(ex);
            }
        }

        private void ShowInfomation(Exception ex)
        {
            ShowInfomation(ex.Message, true);
        }

        private void ShowInfomation(string message, bool isErr = false)
        {
            if (!string.IsNullOrEmpty(txtShowInfo.Text))
                txtShowInfo.AppendText(Environment.NewLine);

            txtShowInfo.AppendText(message);
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}