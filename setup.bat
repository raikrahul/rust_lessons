@echo off
REM Create necessary directory structure

REM Create the "assets/css" directory for your CSS
mkdir assets
mkdir assets\css

REM Create "_posts" directory for tutorial posts
mkdir _posts

REM Create ".github/workflows" directory for GitHub Actions
mkdir .github
mkdir .github\workflows

REM Move style.css to assets/css
if exist style.css (
    move style.css assets\css\
) else (
    echo style.css not found, skipping move.
)

REM Rename index.html to index.md
if exist index.html (
    ren index.html index.md
) else (
    echo index.html not found, skipping rename.
)

REM Create a sample jekyll-gh-pages.yml file in .github/workflows
echo name: Deploy Jekyll with GitHub Pages dependencies preinstalled > .github\workflows\jekyll-gh-pages.yml
echo. >> .github\workflows\jekyll-gh-pages.yml
echo on: >> .github\workflows\jekyll-gh-pages.yml
echo   push: >> .github\workflows\jekyll-gh-pages.yml
echo     branches: ["main"] >> .github\workflows\jekyll-gh-pages.yml
echo   workflow_dispatch: >> .github\workflows\jekyll-gh-pages.yml
echo. >> .github\workflows\jekyll-gh-pages.yml
echo permissions: >> .github\workflows\jekyll-gh-pages.yml
echo   contents: read >> .github\workflows\jekyll-gh-pages.yml
echo   pages: write >> .github\workflows\jekyll-gh-pages.yml
echo   id-token: write >> .github\workflows\jekyll-gh-pages.yml
echo. >> .github\workflows\jekyll-gh-pages.yml
echo concurrency: >> .github\workflows\jekyll-gh-pages.yml
echo   group: "pages" >> .github\workflows\jekyll-gh-pages.yml
echo   cancel-in-progress: false >> .github\workflows\jekyll-gh-pages.yml
echo. >> .github\workflows\jekyll-gh-pages.yml
echo jobs: >> .github\workflows\jekyll-gh-pages.yml
echo   build: >> .github\workflows\jekyll-gh-pages.yml
echo     runs-on: ubuntu-latest >> .github\workflows\jekyll-gh-pages.yml
echo     steps: >> .github\workflows\jekyll-gh-pages.yml
echo       - name: Checkout >> .github\workflows\jekyll-gh-pages.yml
echo         uses: actions/checkout@v4 >> .github\workflows\jekyll-gh-pages.yml
echo       - name: Setup Pages >> .github\workflows\jekyll-gh-pages.yml
echo         uses: actions/configure-pages@v5 >> .github\workflows\jekyll-gh-pages.yml
echo       - name: Build with Jekyll >> .github\workflows\jekyll-gh-pages.yml
echo         uses: actions/jekyll-build-pages@v1 >> .github\workflows\jekyll-gh-pages.yml
echo         with: >> .github\workflows\jekyll-gh-pages.yml
echo           source: ./ >> .github\workflows\jekyll-gh-pages.yml
echo           destination: ./_site >> .github\workflows\jekyll-gh-pages.yml
echo       - name: Upload artifact >> .github\workflows\jekyll-gh-pages.yml
echo         uses: actions/upload-pages-artifact@v3 >> .github\workflows\jekyll-gh-pages.yml
echo. >> .github\workflows\jekyll-gh-pages.yml
echo   deploy: >> .github\workflows\jekyll-gh-pages.yml
echo     environment: >> .github\workflows\jekyll-gh-pages.yml
echo       name: github-pages >> .github\workflows\jekyll-gh-pages.yml
echo       url: ${{ steps.deployment.outputs.page_url }} >> .github\workflows\jekyll-gh-pages.yml
echo     runs-on: ubuntu-latest >> .github\workflows\jekyll-gh-pages.yml
echo     needs: build >> .github\workflows\jekyll-gh-pages.yml
echo     steps: >> .github\workflows\jekyll-gh-pages.yml
echo       - name: Deploy to GitHub Pages >> .github\workflows\jekyll-gh-pages.yml
echo         id: deployment >> .github\workflows\jekyll-gh-pages.yml
echo         uses: actions/deploy-pages@v4 >> .github\workflows\jekyll-gh-pages.yml

echo Directory setup complete! 
pause
