+# QUIZZO - Online Quiz System

**QUIZZO** is a simple, offline-compatible online quiz system built using HTML, CSS, JavaScript (frontend), C with CGI for backend processing, and Python for advanced search functionality. It runs locally using **XAMPP** for Apache hosting and CGI support.

---

## 📁 Folder Structure
QUIZZO/
├── cgi-bin/            <-- Compiled CGI executables (.cgi)
├── www/                <-- ✅ All QUIZZO frontend files (HTML/CSS/JS)
├── quiz_data.json      <-- JSON file for quiz search
├── search_server.py    <-- Python Flask search backend
├── README.md           <-- Setup + usage instructions


---

## ⚙️ Requirements

- **XAMPP** (Apache with CGI enabled)
- **Python 3.x**
  - Install `Flask` and `flask-cors`:
    ```bash
    pip install flask flask-cors
    ```

---

## 🚀 How to Run

###  1. Set up XAMPP for CGI
1. Install XAMPP (if not already).
2. Copy the contents of this folder to your XAMPP installation:
   - Copy `cgi-bin/` contents to `C:/xampp/cgi-bin/`
   - Locate `htdocs`. Create the folder `QUIZZO` inside it, followed by `www` inside `QUIZZO`
   - Copy `www/` contents to `C:/xampp/htdocs/QUIZZO/www/`
   - Copy `quiz_data.json` to `C:/xampp/htdocs/QUIZZO/data/`
3. Start Apache from XAMPP Control Panel.
4. Open your browser and visit:
   http://localhost/QUIZZO/login.html
5. Click on sign-up and enter the following user credentials:
testuser
testemail@gmail.com
testpassword
confirm password: testpassword
9876543210
BTech
2401CS01

- Ensure `httpd.conf` contains:
  ```apache
  ScriptAlias /cgi-bin/ "C:/xampp/cgi-bin/"
  <Directory "C:/xampp/cgi-bin">
      AllowOverride None
      Options +ExecCGI
      Require all granted
      AddHandler cgi-script .cgi .exe
  </Directory>

### 2. Run the python search server
 ```bash
    cd path/to/QUIZZO
    python search_server.py
    ```


🧠 Features
1. 🔐 Signup and login using CGI (C backend)

2. 🧑 User dashboard with:
   - Profile (username, rank, score)
   - Leaderboard (dynamically updated)
   - Quizzes (clickable links)
   - 🔎 Live search bar for quizzes (tag-based search via Python Flask)
   - ✨ Ambient UI with scrollable dropdowns and filters

📌 Notes
1. All user data is stored in users.txt

2. Rating and rank updates are calculated based on quiz performance

3. Search supports partial title or tag matching

