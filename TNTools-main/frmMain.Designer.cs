namespace TNTools
{
    partial class frmMain
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMain));
            groupBox1 = new GroupBox();
            numGCPCode = new NumericUpDown();
            btnGenerateGLNAndGCPCode = new Button();
            numGLNCode = new NumericUpDown();
            txtUsername = new TextBox();
            label5 = new Label();
            label2 = new Label();
            label1 = new Label();
            txtShowInfo = new TextBox();
            btnClose = new Button();
            groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)numGCPCode).BeginInit();
            ((System.ComponentModel.ISupportInitialize)numGLNCode).BeginInit();
            SuspendLayout();
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(numGCPCode);
            groupBox1.Controls.Add(btnGenerateGLNAndGCPCode);
            groupBox1.Controls.Add(numGLNCode);
            groupBox1.Controls.Add(txtUsername);
            groupBox1.Controls.Add(label5);
            groupBox1.Controls.Add(label2);
            groupBox1.Controls.Add(label1);
            groupBox1.Location = new Point(12, 12);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new Size(343, 127);
            groupBox1.TabIndex = 0;
            groupBox1.TabStop = false;
            groupBox1.Text = "Tạo mã GLN và GCP";
            // 
            // numGCPCode
            // 
            numGCPCode.Location = new Point(112, 90);
            numGCPCode.Name = "numGCPCode";
            numGCPCode.Size = new Size(90, 23);
            numGCPCode.TabIndex = 2;
            numGCPCode.Value = new decimal(new int[] { 1, 0, 0, 0 });
            // 
            // btnGenerateGLNAndGCPCode
            // 
            btnGenerateGLNAndGCPCode.BackColor = Color.Lime;
            btnGenerateGLNAndGCPCode.Font = new Font("Segoe UI", 10F, FontStyle.Bold, GraphicsUnit.Point);
            btnGenerateGLNAndGCPCode.Location = new Point(232, 60);
            btnGenerateGLNAndGCPCode.Name = "btnGenerateGLNAndGCPCode";
            btnGenerateGLNAndGCPCode.Size = new Size(105, 53);
            btnGenerateGLNAndGCPCode.TabIndex = 3;
            btnGenerateGLNAndGCPCode.Text = "Tạo mã";
            btnGenerateGLNAndGCPCode.UseVisualStyleBackColor = false;
            btnGenerateGLNAndGCPCode.Click += btnCreateGcpAndGln_Click;
            // 
            // numGLNCode
            // 
            numGLNCode.Location = new Point(112, 60);
            numGLNCode.Name = "numGLNCode";
            numGLNCode.Size = new Size(90, 23);
            numGLNCode.TabIndex = 1;
            numGLNCode.Value = new decimal(new int[] { 1, 0, 0, 0 });
            // 
            // txtUsername
            // 
            txtUsername.Location = new Point(112, 31);
            txtUsername.Name = "txtUsername";
            txtUsername.Size = new Size(225, 23);
            txtUsername.TabIndex = 0;
            // 
            // label5
            // 
            label5.AutoSize = true;
            label5.Location = new Point(6, 92);
            label5.Name = "label5";
            label5.Size = new Size(100, 15);
            label5.TabIndex = 3;
            label5.Text = "Số lượng mã GCP";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(6, 62);
            label2.Name = "label2";
            label2.Size = new Size(100, 15);
            label2.TabIndex = 3;
            label2.Text = "Số lượng mã GLN";
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(6, 34);
            label1.Name = "label1";
            label1.Size = new Size(85, 15);
            label1.TabIndex = 3;
            label1.Text = "Tên đăng nhập";
            // 
            // txtShowInfo
            // 
            txtShowInfo.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            txtShowInfo.Location = new Point(12, 145);
            txtShowInfo.Multiline = true;
            txtShowInfo.Name = "txtShowInfo";
            txtShowInfo.ReadOnly = true;
            txtShowInfo.Size = new Size(897, 332);
            txtShowInfo.TabIndex = 2;
            // 
            // btnClose
            // 
            btnClose.BackColor = Color.FromArgb(128, 128, 255);
            btnClose.Font = new Font("Segoe UI", 10F, FontStyle.Bold, GraphicsUnit.Point);
            btnClose.Location = new Point(804, 483);
            btnClose.Name = "btnClose";
            btnClose.Size = new Size(105, 51);
            btnClose.TabIndex = 1;
            btnClose.Text = "Đóng";
            btnClose.UseVisualStyleBackColor = false;
            btnClose.Click += btnClose_Click;
            // 
            // frmMain
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(921, 542);
            Controls.Add(txtShowInfo);
            Controls.Add(btnClose);
            Controls.Add(groupBox1);
            FormBorderStyle = FormBorderStyle.FixedSingle;
            Icon = (Icon)resources.GetObject("$this.Icon");
            MaximizeBox = false;
            MinimizeBox = false;
            Name = "frmMain";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "TN Tools";
            Load += frmMain_Load;
            groupBox1.ResumeLayout(false);
            groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)numGCPCode).EndInit();
            ((System.ComponentModel.ISupportInitialize)numGLNCode).EndInit();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private GroupBox groupBox1;
        private Button btnGenerateGLNAndGCPCode;
        private TextBox txtUsername;
        private Label label1;
        private TextBox txtShowInfo;
        private NumericUpDown numGLNCode;
        private Label label2;
        private NumericUpDown numGCPCode;
        private Button btnClose;
        private Label label5;
    }
}