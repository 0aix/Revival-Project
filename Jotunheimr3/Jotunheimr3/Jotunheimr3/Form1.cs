using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Jotunheimr3
{
    public partial class Form1 : Form
    {
        string root;
        int rootlength;
        List<string> TextStrings = new List<string>();

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = folderBrowserDialog1.SelectedPath;
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (Directory.Exists(textBox1.Text))
            {
                button1.Enabled = false;
                button2.Enabled = false;
                root = textBox1.Text;
                rootlength = root.Length;
                openFileDialog1.InitialDirectory = root;
                button3.Enabled = true;
                button4.Enabled = true;
                button5.Enabled = true;
                listBox1.Enabled = true;
                textBox2.Enabled = true;
                richTextBox1.Enabled = true;
            }
        }
        
        private void button5_Click(object sender, EventArgs e)
        {
            if (!String.IsNullOrEmpty(richTextBox1.Text))
            {
                string s = richTextBox1.Text;
                TextStrings.Add(s);
                listBox1.Items.Add(s);
                listBox1.SelectedIndex = listBox1.Items.Count - 1;
            }
            else
            {
                MessageBox.Show("String must not be empty");
            }
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex >= 0)
            {
                string s = TextStrings[listBox1.SelectedIndex];
                richTextBox1.Text = s;
            }
            else
            {
                richTextBox1.Text = "";
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            listBox1.Items.Clear();
            TextStrings.Clear();
            richTextBox1.Text = "";
        }

        private void listBox1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
            {
                if (listBox1.SelectedIndex >= 0 && listBox1.Items.Count > 0)
                {
                    int index = listBox1.SelectedIndex;
                    TextStrings.RemoveAt(index);
                    listBox1.Items.RemoveAt(index);
                }
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            try
            {
                if (openFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    FileStream fs = File.Create(openFileDialog1.FileName);
                    fs.Write(new byte[] { 84, 69, 88, 84 }, 0, 4); //TEXT
                    fs.WriteByte((byte)textBox2.Text.Length);
                    fs.Write(Encoding.UTF8.GetBytes(textBox2.Text), 0, textBox2.Text.Length);
                    int count = TextStrings.Count;
                    fs.Write(BitConverter.GetBytes(count), 0, 1);
                    for (int i = 0; i < count; i++)
                    {
                        string s = TextStrings[i];
                        fs.Write(BitConverter.GetBytes(s.Length), 0, 2);
                        fs.Write(Encoding.UTF8.GetBytes(s), 0, s.Length);
                    }
                    fs.Flush();
                    fs.Close();
                    openFileDialog1.InitialDirectory = root;
                    MessageBox.Show("Written successfully");
                }
            }
            catch (Exception z) { MessageBox.Show("Error"); }
        }
    }
}
