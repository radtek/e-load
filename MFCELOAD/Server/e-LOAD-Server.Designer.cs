namespace Server
{
     partial class frmeLOAD
     {
          /// <summary>
          /// Required designer variable.
          /// </summary>
          private System.ComponentModel.IContainer components = null;

          /// <summary>
          /// Clean up any resources being used.
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
          /// Required method for Designer support - do not modify
          /// the contents of this method with the code editor.
          /// </summary>
          private void InitializeComponent()
          {
			  this.groupBox1 = new System.Windows.Forms.GroupBox();
			  this.textVersionBox = new System.Windows.Forms.TextBox();
			  this.label4 = new System.Windows.Forms.Label();
			  this.buttonDelete = new System.Windows.Forms.Button();
			  this.buttonAdd = new System.Windows.Forms.Button();
			  this.textAddressBox = new System.Windows.Forms.TextBox();
			  this.textNameBox = new System.Windows.Forms.TextBox();
			  this.label3 = new System.Windows.Forms.Label();
			  this.label2 = new System.Windows.Forms.Label();
			  this.listUser = new System.Windows.Forms.ListView();
			  this.buttonGet = new System.Windows.Forms.Button();
			  this.textURLBox = new System.Windows.Forms.TextBox();
			  this.label1 = new System.Windows.Forms.Label();
			  this.buttonSave = new System.Windows.Forms.Button();
			  this.groupBox1.SuspendLayout();
			  this.SuspendLayout();
			  // 
			  // groupBox1
			  // 
			  this.groupBox1.Controls.Add(this.textVersionBox);
			  this.groupBox1.Controls.Add(this.label4);
			  this.groupBox1.Controls.Add(this.buttonDelete);
			  this.groupBox1.Controls.Add(this.buttonAdd);
			  this.groupBox1.Controls.Add(this.textAddressBox);
			  this.groupBox1.Controls.Add(this.textNameBox);
			  this.groupBox1.Controls.Add(this.label3);
			  this.groupBox1.Controls.Add(this.label2);
			  this.groupBox1.Controls.Add(this.listUser);
			  this.groupBox1.Controls.Add(this.buttonGet);
			  this.groupBox1.Controls.Add(this.textURLBox);
			  this.groupBox1.Controls.Add(this.label1);
			  this.groupBox1.Location = new System.Drawing.Point(12, 12);
			  this.groupBox1.Name = "groupBox1";
			  this.groupBox1.Size = new System.Drawing.Size(566, 567);
			  this.groupBox1.TabIndex = 0;
			  this.groupBox1.TabStop = false;
			  this.groupBox1.Text = "e-LOAD Server Setting";
			  // 
			  // textVersionBox
			  // 
			  this.textVersionBox.Location = new System.Drawing.Point(53, 51);
			  this.textVersionBox.Name = "textVersionBox";
			  this.textVersionBox.Size = new System.Drawing.Size(131, 21);
			  this.textVersionBox.TabIndex = 11;
			  // 
			  // label4
			  // 
			  this.label4.AutoSize = true;
			  this.label4.Location = new System.Drawing.Point(9, 61);
			  this.label4.Name = "label4";
			  this.label4.Size = new System.Drawing.Size(41, 12);
			  this.label4.TabIndex = 10;
			  this.label4.Text = "버젼 : ";
			  // 
			  // buttonDelete
			  // 
			  this.buttonDelete.Location = new System.Drawing.Point(485, 142);
			  this.buttonDelete.Name = "buttonDelete";
			  this.buttonDelete.Size = new System.Drawing.Size(75, 23);
			  this.buttonDelete.TabIndex = 9;
			  this.buttonDelete.Text = "삭제";
			  this.buttonDelete.UseVisualStyleBackColor = true;
			  this.buttonDelete.Click += new System.EventHandler(this.buttonDelete_Click);
			  // 
			  // buttonAdd
			  // 
			  this.buttonAdd.Location = new System.Drawing.Point(391, 142);
			  this.buttonAdd.Name = "buttonAdd";
			  this.buttonAdd.Size = new System.Drawing.Size(75, 23);
			  this.buttonAdd.TabIndex = 8;
			  this.buttonAdd.Text = "추가";
			  this.buttonAdd.UseVisualStyleBackColor = true;
			  this.buttonAdd.Click += new System.EventHandler(this.buttonAdd_Click);
			  // 
			  // textAddressBox
			  // 
			  this.textAddressBox.Location = new System.Drawing.Point(129, 142);
			  this.textAddressBox.Name = "textAddressBox";
			  this.textAddressBox.Size = new System.Drawing.Size(255, 21);
			  this.textAddressBox.TabIndex = 7;
			  // 
			  // textNameBox
			  // 
			  this.textNameBox.Location = new System.Drawing.Point(9, 142);
			  this.textNameBox.Name = "textNameBox";
			  this.textNameBox.Size = new System.Drawing.Size(100, 21);
			  this.textNameBox.TabIndex = 6;
			  // 
			  // label3
			  // 
			  this.label3.AutoSize = true;
			  this.label3.Location = new System.Drawing.Point(127, 117);
			  this.label3.Name = "label3";
			  this.label3.Size = new System.Drawing.Size(57, 12);
			  this.label3.TabIndex = 5;
			  this.label3.Text = "물리 주소";
			  // 
			  // label2
			  // 
			  this.label2.AutoSize = true;
			  this.label2.Location = new System.Drawing.Point(9, 117);
			  this.label2.Name = "label2";
			  this.label2.Size = new System.Drawing.Size(29, 12);
			  this.label2.TabIndex = 4;
			  this.label2.Text = "이름";
			  // 
			  // listUser
			  // 
			  this.listUser.CheckBoxes = true;
			  this.listUser.GridLines = true;
			  this.listUser.Location = new System.Drawing.Point(9, 174);
			  this.listUser.Name = "listUser";
			  this.listUser.Size = new System.Drawing.Size(551, 387);
			  this.listUser.Sorting = System.Windows.Forms.SortOrder.Ascending;
			  this.listUser.TabIndex = 3;
			  this.listUser.UseCompatibleStateImageBehavior = false;
			  this.listUser.View = System.Windows.Forms.View.Details;
			  this.listUser.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.listUser_ColumnClick);
			  // 
			  // buttonGet
			  // 
			  this.buttonGet.Location = new System.Drawing.Point(485, 23);
			  this.buttonGet.Name = "buttonGet";
			  this.buttonGet.Size = new System.Drawing.Size(75, 23);
			  this.buttonGet.TabIndex = 2;
			  this.buttonGet.Text = "Get";
			  this.buttonGet.UseVisualStyleBackColor = true;
			  this.buttonGet.Click += new System.EventHandler(this.buttonGet_Click);
			  // 
			  // textURLBox
			  // 
			  this.textURLBox.Location = new System.Drawing.Point(53, 24);
			  this.textURLBox.Name = "textURLBox";
			  this.textURLBox.Size = new System.Drawing.Size(426, 21);
			  this.textURLBox.TabIndex = 1;
			  // 
			  // label1
			  // 
			  this.label1.AutoSize = true;
			  this.label1.Location = new System.Drawing.Point(7, 29);
			  this.label1.Name = "label1";
			  this.label1.Size = new System.Drawing.Size(40, 12);
			  this.label1.TabIndex = 0;
			  this.label1.Text = "URL : ";
			  // 
			  // buttonSave
			  // 
			  this.buttonSave.Location = new System.Drawing.Point(497, 585);
			  this.buttonSave.Name = "buttonSave";
			  this.buttonSave.Size = new System.Drawing.Size(81, 36);
			  this.buttonSave.TabIndex = 1;
			  this.buttonSave.Text = "저장";
			  this.buttonSave.UseVisualStyleBackColor = true;
			  this.buttonSave.Click += new System.EventHandler(this.buttonSave_Click);
			  // 
			  // frmeLOAD
			  // 
			  this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			  this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			  this.ClientSize = new System.Drawing.Size(590, 629);
			  this.Controls.Add(this.buttonSave);
			  this.Controls.Add(this.groupBox1);
			  this.MaximizeBox = false;
			  this.Name = "frmeLOAD";
			  this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			  this.Text = "e-LOAD Server";
			  this.TopMost = true;
			  this.Load += new System.EventHandler(this.frmeLOAD_Load);
			  this.groupBox1.ResumeLayout(false);
			  this.groupBox1.PerformLayout();
			  this.ResumeLayout(false);

          }

          #endregion

          private System.Windows.Forms.GroupBox groupBox1;
          private System.Windows.Forms.Button buttonGet;
          private System.Windows.Forms.TextBox textURLBox;
          private System.Windows.Forms.Label label1;
          private System.Windows.Forms.ListView listUser;
          private System.Windows.Forms.Button buttonAdd;
          private System.Windows.Forms.TextBox textAddressBox;
          private System.Windows.Forms.TextBox textNameBox;
          private System.Windows.Forms.Label label3;
          private System.Windows.Forms.Label label2;
          private System.Windows.Forms.Button buttonDelete;
          private System.Windows.Forms.TextBox textVersionBox;
          private System.Windows.Forms.Label label4;
          private System.Windows.Forms.Button buttonSave;
     }
}

