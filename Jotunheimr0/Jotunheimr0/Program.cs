using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace Jotunheimr0
{
    class TO
    {
        public static int SPRITE = 0;
        public static int SOUND = 1;
        public static int STRING = 2;
        public static int ANIM = 3;
        public static int PACK = 4;
        public static int LIST = 255;
    };

    class Item
    {
        public byte[] hash;
        public string name;
    }

    class Frame : Item
    {
        public int x;
        public int y;
        public int delay;
    }

    class Anim : Item
    {
        public string animname;
        public byte framecount;
        public byte loops;
        public List<Frame> frames = new List<Frame>();
    }

    class PItem : Item
    {
        public int type; //Same as table type
    }

    class Pack : Item
    {
        public string packname;
        public int itemcount;
        public List<PItem> items = new List<PItem>();
    }

    class Program
    {
        [DllImport("msvcrt.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern int memcmp(byte[] b1, byte[] b2, long count);

        static MD5 md5 = MD5.Create();
        static FileStream SpriteTO;
        static FileStream SoundTO;
        static FileStream StringTO;
        static FileStream AnimTO;
        static FileStream PackTO;
        static FileStream ListTO;
        static FileStream TOH;
        static byte version;

        static string root;
        static int root_length;
        static string cd;
        static Stack<string> DFS = new Stack<string>();
        static List<Item> PNG = new List<Item>();
        static List<Item> OGG = new List<Item>();
        static List<string> STRING = new List<string>();
        static List<Anim> ANIM = new List<Anim>();
        static List<Pack> PACK = new List<Pack>();
        static int stringid = -1;

        static List<int>[] FileSize = new List<int>[5];

        static bool CompareHash(byte[] hash1, byte[] hash2)
        {
            for (int i = 0; i < 16; i++)
                if (hash1[i] != hash2[i])
                    return false;
            return true;
        }

        static void GlobalHeader(FileStream fs)
        {
            fs.Write(new byte[] { 84, 82, 80, 72 }, 0, 4); //TRPH
            fs.WriteByte((byte)version);
            fs.Write(new byte[] { 0, 0, 0, 0 }, 0, 3); //Reserved bytes
            fs.Flush();
        }

        static void OpenTO()
        {
            SpriteTO = File.Create(root + "\\Sprite.to");
            SoundTO = File.Create(root + "\\Sound.to");
            StringTO = File.Create(root + "\\String.to");
            AnimTO = File.Create(root + "\\Anim.to");
            PackTO = File.Create(root + "\\Pack.to");
            ListTO = File.Create(root + "\\List.to");
            GlobalHeader(SpriteTO);
            GlobalHeader(SoundTO);
            GlobalHeader(StringTO);
            GlobalHeader(AnimTO);
            GlobalHeader(PackTO);
            GlobalHeader(ListTO);
        }

        static void CloseTO()
        {
            SpriteTO.Seek(5, SeekOrigin.Begin);
            SpriteTO.WriteByte((byte)TO.SPRITE);
            SpriteTO.Write(BitConverter.GetBytes(PNG.Count), 0, 2);
            SpriteTO.Close();

            SoundTO.Seek(5, SeekOrigin.Begin);
            SoundTO.WriteByte((byte)TO.SOUND);
            SoundTO.Write(BitConverter.GetBytes(OGG.Count), 0, 2);
            SoundTO.Close();

            StringTO.Seek(5, SeekOrigin.Begin);
            StringTO.WriteByte((byte)TO.STRING);
            StringTO.Write(BitConverter.GetBytes(STRING.Count), 0, 2);
            StringTO.Close();

            AnimTO.Seek(5, SeekOrigin.Begin);
            AnimTO.WriteByte((byte)TO.ANIM);
            AnimTO.Write(BitConverter.GetBytes(ANIM.Count), 0, 2);
            AnimTO.Close();

            PackTO.Seek(5, SeekOrigin.Begin);
            PackTO.WriteByte((byte)TO.PACK);
            PackTO.Write(BitConverter.GetBytes(PACK.Count), 0, 2);
            PackTO.Close();

            ListTO.Seek(5, SeekOrigin.Begin);
            ListTO.WriteByte((byte)TO.LIST);
            ListTO.Write(BitConverter.GetBytes(5), 0, 2);
            ListTO.Close();
        }

        static void WriteStringTO()
        {
            int flush = 0;
            for (int i = 0; i < STRING.Count; i++)
            {
                StringTO.Write(Encoding.UTF8.GetBytes(STRING[i]), 0, STRING[i].Length);
                FileSize[TO.STRING].Add(STRING[i].Length);

                flush += STRING[i].Length;
                if (flush >= 4096)
                {
                    StringTO.Flush();
                    flush = 0;
                }
            }
            StringTO.Flush();
        }

        static int FindItem(int length, byte[] hash, int type)
        {
            if (type == TO.SPRITE)
            {
                for (int z = 0; z < PNG.Count; z++)
                    if (PNG[z].name.Length == length && CompareHash(PNG[z].hash, hash))
                        return z;
            }
            else if (type == TO.SOUND)
            {
                for (int z = 0; z < OGG.Count; z++)
                    if (OGG[z].name.Length == length && CompareHash(OGG[z].hash, hash))
                        return z;
            }
            else if (type == TO.STRING)
            {
                stringid++;
                return stringid;
            }
            else if (type == TO.ANIM)
            {
                for (int z = 0; z < ANIM.Count; z++)
                    if (ANIM[z].name.Length == length && CompareHash(ANIM[z].hash, hash))
                        return z;
            }
            else if (type == TO.PACK)
            {
                for (int z = 0; z < PACK.Count; z++)
                    if (PACK[z].name.Length == length && CompareHash(PACK[z].hash, hash))
                        return z;
            }
            return -1;
        }

        static bool WriteAnimTO()
        {
            for (int i = 0; i < ANIM.Count; i++)
            {
                Anim a = ANIM[i];
                AnimTO.WriteByte((byte)a.framecount);
                AnimTO.WriteByte(a.loops);
                for (int n = 0; n < a.framecount; n++)
                {
                    byte[] hash = a.frames[n].hash;
                    int length = a.frames[n].name.Length;
                    int id = FindItem(length, hash, TO.SPRITE);
                    if (id == -1)
                        return false;
                    AnimTO.Write(BitConverter.GetBytes(id), 0, 2);
                    AnimTO.Write(BitConverter.GetBytes(a.frames[n].x), 0, 2);
                    AnimTO.Write(BitConverter.GetBytes(a.frames[n].y), 0, 2);
                    AnimTO.Write(BitConverter.GetBytes(a.frames[n].delay), 0, 2);
                }
                AnimTO.Flush();
                FileSize[TO.ANIM].Add(2 + 8 * a.framecount);
            }
            return true;
        }

        static bool WritePackTO()
        {
            for (int i = 0; i < PACK.Count; i++)
            {
                Pack p = PACK[i];
                PackTO.WriteByte((byte)p.itemcount);
                for (int n = 0; n < p.itemcount; n++)
                {
                    byte[] hash = p.items[n].hash;
                    int length = p.items[n].name.Length;
                    PackTO.WriteByte((byte)p.items[n].type);
                    int id = FindItem(length, hash, p.items[n].type);
                    if (id == -1)
                        return false;
                    PackTO.Write(BitConverter.GetBytes(id), 0, 2);
                }
                PackTO.Flush();
                FileSize[TO.PACK].Add(1 + 3 * p.itemcount);
            }
            return true;
        }

        static void WriteListTO()
        {
            for (int i = 0; i < 5; i++)
            {
                List<int> size = FileSize[i];
                int count = size.Count;
                ListTO.WriteByte((byte)i);
                ListTO.Write(BitConverter.GetBytes(count), 0, 2);
                int largest = 0;
                int numbytes = 0;
                for (int n = 0; n < count; n++)
                    if (size[n] > largest)
                        largest = size[n];
                if (largest <= 0xFF)
                    numbytes = 1;
                else if (largest <= 0xFFFF)
                    numbytes = 2;
                else if (largest <= 0xFFFFFF)
                    numbytes = 3;
                else
                    numbytes = 4;
                ListTO.WriteByte((byte)numbytes);
                for (int n = 0; n < count; n++)
                    ListTO.Write(BitConverter.GetBytes(size[n]), 0, numbytes);
                ListTO.Flush();
            }
        }

        static void WriteFile(FileStream fs, FileInfo fi, int length)
        {
            FileStream readfs = fi.OpenRead();
            byte[] buffer = new byte[length];
            readfs.Read(buffer, 0, length);
            fs.Write(buffer, 0, length);
            fs.Flush();
        }

        //REQ: DFS not empty
        static bool DFSNext()
        {
            cd = DFS.Pop();
            foreach (string s in Directory.EnumerateDirectories(cd))
                DFS.Push(s);

            foreach (string s in Directory.EnumerateFiles(cd))
            {
                FileInfo fi = new FileInfo(s);
                string relpath = fi.FullName.Substring(root_length);
                if (fi.Extension.Equals(".png"))
                {
                    Item item = new Item();
                    item.hash = md5.ComputeHash(Encoding.UTF8.GetBytes(relpath));
                    item.name = relpath;
                    WriteFile(SpriteTO, fi, (int)fi.Length);
                    FileSize[TO.SPRITE].Add((int)fi.Length);
                    PNG.Add(item);
                }
                else if (fi.Extension.Equals(".ogg"))
                {
                    Item item = new Item();
                    item.hash = md5.ComputeHash(Encoding.UTF8.GetBytes(relpath));
                    item.name = relpath;
                    WriteFile(SoundTO, fi, (int)fi.Length);
                    FileSize[TO.SOUND].Add((int)fi.Length);
                    OGG.Add(item);
                }
                else if (fi.Extension.Equals(".anim"))
                {
                    Anim anim = new Anim();
                    anim.hash = md5.ComputeHash(Encoding.UTF8.GetBytes(relpath));
                    anim.name = relpath;

                    FileStream fs = fi.OpenRead();
                    fs.Seek(4, SeekOrigin.Begin);
                    int namelength = fs.ReadByte();
                    byte[] buffer = new byte[namelength];
                    fs.Read(buffer, 0, namelength);
                    anim.animname = Encoding.UTF8.GetString(buffer);
                    anim.framecount = (byte)fs.ReadByte();
                    anim.loops = (byte)fs.ReadByte();
                    for (int i = 0; i < anim.framecount; i++)
                    {
                        int flength = fs.ReadByte();
                        Frame frame = new Frame();
                        byte[] fbuffer = new byte[flength];
                        fs.Read(fbuffer, 0, flength);
                        frame.name = Encoding.UTF8.GetString(fbuffer);
                        frame.hash = md5.ComputeHash(fbuffer);
                        byte[] sbuffer = new byte[2];
                        fs.Read(sbuffer, 0, 2);
                        frame.x = BitConverter.ToInt16(sbuffer, 0);
                        fs.Read(sbuffer, 0, 2);
                        frame.y = BitConverter.ToInt16(sbuffer, 0);
                        fs.Read(sbuffer, 0, 2);
                        frame.delay = BitConverter.ToInt16(sbuffer, 0);
                        anim.frames.Add(frame);
                    }
                    fs.Close();

                    if (anim.animname.Length > 0)
                        WriteEntry(anim.animname, ANIM.Count);

                    ANIM.Add(anim);

                }
                else if (fi.Extension.Equals(".pack"))
                {
                    Pack pack = new Pack();
                    pack.hash = md5.ComputeHash(Encoding.UTF8.GetBytes(relpath));
                    pack.name = relpath;

                    FileStream fs = fi.OpenRead();
                    fs.Seek(4, SeekOrigin.Begin);
                    int namelength = fs.ReadByte();
                    byte[] buffer = new byte[namelength];
                    fs.Read(buffer, 0, namelength);
                    pack.packname = Encoding.UTF8.GetString(buffer);
                    pack.itemcount = fs.ReadByte();
                    for (int i = 0; i < pack.itemcount; i++)
                    {
                        PItem item = new PItem();
                        int pathlength = fs.ReadByte();
                        byte[] pathbuffer = new byte[pathlength];
                        fs.Read(pathbuffer, 0, pathlength);
                        item.name = Encoding.UTF8.GetString(pathbuffer);
                        item.hash = md5.ComputeHash(pathbuffer);
                        FileInfo finfo = new FileInfo(root + item.name);
                        if (finfo.Extension.Equals(".png"))
                            item.type = TO.SPRITE;
                        else if (finfo.Extension.Equals(".ogg"))
                            item.type = TO.SOUND;
                        else if (finfo.Extension.Equals(".text"))
                            item.type = TO.STRING;
                        else if (finfo.Extension.Equals(".anim"))
                            item.type = TO.ANIM;
                        else if (finfo.Extension.Equals(".pack"))
                            item.type = TO.PACK;
                        pack.items.Add(item);
                    }
                    fs.Close();

                    if (pack.packname.Length > 0)
                        WriteEntry(pack.packname, PACK.Count);

                    PACK.Add(pack);
                }
                else if (fi.Extension.Equals(".text"))
                {
                    Pack pack = new Pack();
                    pack.hash = md5.ComputeHash(Encoding.UTF8.GetBytes(relpath));
                    pack.name = relpath;

                    FileStream fs = fi.OpenRead();
                    fs.Seek(4, SeekOrigin.Begin);
                    int namelength = fs.ReadByte();
                    byte[] buffer = new byte[namelength];
                    fs.Read(buffer, 0, namelength);
                    pack.packname = Encoding.UTF8.GetString(buffer);
                    pack.itemcount = fs.ReadByte();
                    for (int i = 0; i < pack.itemcount; i++)
                    {
                        PItem item = new PItem();
                        byte[] lbuffer = new byte[2];
                        fs.Read(lbuffer, 0, 2);
                        int slength = BitConverter.ToInt16(lbuffer, 0);
                        byte[] sbuffer = new byte[slength];
                        fs.Read(sbuffer, 0, slength);
                        STRING.Add(Encoding.UTF8.GetString(sbuffer));
                        item.type = 5;
                        pack.items.Add(item);
                    }
                    fs.Close();

                    if (pack.packname.Length > 0)
                        WriteEntry(pack.packname, PACK.Count);

                    PACK.Add(pack);
                }
            }

            if (DFS.Count == 0)
                return false;

            return true;
        }

        static void WriteHeader()
        {
            TOH = File.Create(root + "\\TO.h");
            TOH.Write(Encoding.UTF8.GetBytes("#ifndef TO_H\n#define TO_H\n\nnamespace TO\n{\n"), 0, 42);
            TOH.Flush();
        }

        static void WriteEntry(string name, int value)
        {
            string s = "\tconst int " + name + " = " + value.ToString() + ";\n";
            TOH.Write(Encoding.UTF8.GetBytes(s), 0, s.Length);
            TOH.Flush();
        }

        static void WriteFooter()
        {
            TOH.Write(Encoding.UTF8.GetBytes("}\n\n#endif"), 0, 9);
            TOH.Flush();
            TOH.Close();
        }

        static void Main(string[] args)
        {
            Console.Title = "Jotunheimr0";
            Console.Write("Root: ");
            root = Console.ReadLine();
            Console.Write("Version: ");
            version = byte.Parse(Console.ReadLine());

            if (root.Equals(""))
                root = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            root = root.TrimEnd('\\');

            if (Directory.Exists(root))
            {
                for (int i = 0; i < 5; i++)
                    FileSize[i] = new List<int>();

                OpenTO();
                WriteHeader();

                root_length = root.Length;
                DFS.Push(root);
                while (DFSNext()) { }

                if (!WriteAnimTO())
                {
                    Console.WriteLine("Anim error");
                    Console.ReadKey();
                    return;
                }
                if (!WritePackTO())
                {
                    Console.WriteLine("Pack error");
                    Console.ReadKey();
                    return;
                }
                WriteStringTO();
                WriteListTO();

                CloseTO();
                WriteFooter();

                Console.WriteLine("Done");
            }
            else
                Console.WriteLine("Can't find path");

            Console.ReadKey();
        }
    }
}
