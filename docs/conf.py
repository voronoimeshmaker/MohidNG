from pathlib import Path

project = "Mohid-NG"
copyright = "2026, Universidade do Estado do Rio de Janeiro"
author = "Instituto Politécnico, Universidade do Estado do Rio de Janeiro"
release = "0.1.0"

extensions = [
    "myst_parser",
    "sphinx_design",
    "sphinx_copybutton",
    "sphinxcontrib.bibtex",
]

myst_enable_extensions = [
    "colon_fence",
    "deflist",
    "dollarmath",
]

source_suffix = {
    ".rst": "restructuredtext",
    ".md": "markdown",
}

master_doc = "index"
language = "en"
exclude_patterns = ["_build", "html", "Thumbs.db", ".DS_Store"]

html_theme = "pydata_sphinx_theme"
html_static_path = ["_static"]
html_css_files = ["css/mohid-ng.css"]
html_favicon = "_static/logo/mohid-ng-logo.png"
html_logo = "_static/logo/mohid-ng-logo.png"

html_theme_options = {
    "logo": {
        "text": "Mohid-NG",
        "image_light": "_static/logo/mohid-ng-logo.png",
        "image_dark": "_static/logo/mohid-ng-logo.png",
    },
    "github_url": "https://github.com/mohidng",
    "navbar_align": "left",
    "show_toc_level": 2,
    "navigation_with_keys": True,
    "use_edit_page_button": False,
    "icon_links": [
        {
            "name": "GitHub",
            "url": "https://github.com/mohidng",
            "icon": "fa-brands fa-github",
            "type": "fontawesome",
        },
        {
            "name": "Contact",
            "url": "mailto:mohidng@hotmail",
            "icon": "fa-solid fa-envelope",
            "type": "fontawesome",
        },
    ],
}

html_context = {
    "default_mode": "light",
}

bibtex_bibfiles = ["references.bib"]

# Placeholder for future Doxygen/Breathe integration.
# breathe_projects = {"MohidNG": "../build/doxygen/xml"}
# breathe_default_project = "MohidNG"
