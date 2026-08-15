(() => {
  // <stdin>
  var TOCManager = class {
    constructor() {
      this.container = document.getElementById("toc-container");
      if (!this.container) return;
      this.position = this.container.dataset.position || "center";
      this.headings = [];
      this.tocLinks = [];
      this.activeParam = null;
      this.initialized = false;
      this.alwaysVisible = true;
      this.init();
    }
    init() {
      if (this.initialized) return;
      this.setupElements();
      if (this.tocLinks.length === 0 && !this.sectionCenterToggle && !this.sideIndicator) {
        return;
      }
      this.setupObserver();
      this.bindEvents();
      if (this.sideIndicator) {
        this.sideIndicator.innerHTML = "";
      }
      if (this.position === "side") {
        this.generateSideBlocks();
        this.updateSideBlocks();
      }
      this.initialized = true;
      this.exposeAPI();
    }
    setupElements() {
      Object.assign(this, {
        centerDropdown: document.getElementById("toc-center-dropdown"),
        centerToggle: document.getElementById("toc-center-toggle"),
        centerTitle: document.getElementById("toc-center-title"),
        sectionCenterToggle: document.getElementById("section-center-toggle"),
        sectionCenterDropdown: document.getElementById("section-center-dropdown"),
        sectionCenterTitle: document.getElementById("section-center-title"),
        sideIndicator: document.getElementById("toc-side-indicator"),
        sideDropdown: document.getElementById("toc-side-dropdown"),
        sideCloseBtn: document.getElementById("toc-side-close")
      });
      const links = document.querySelectorAll("#toc-container nav#TableOfContents a");
      const headingIds = [];
      links.forEach((link) => {
        this.tocLinks.push(link);
        try {
          const urlList = link.href.split("#");
          const id = decodeURIComponent(urlList[urlList.length - 1]);
          if (id) headingIds.push(id);
        } catch (e) {
          if (link.hash.substring(1)) headingIds.push(link.hash.substring(1));
        }
      });
      headingIds.forEach((id) => {
        const el = document.getElementById(id);
        if (el) {
          this.headings.push(el);
        }
      });
    }
    setupObserver() {
      const callback = (entries) => {
        const visibleEntries = entries.filter((entry) => entry.isIntersecting);
        if (visibleEntries.length > 0) {
          let topEntry = visibleEntries[0];
          visibleEntries.forEach((entry) => {
            if (entry.boundingClientRect.top < topEntry.boundingClientRect.top) {
              topEntry = entry;
            }
          });
          this.setActive(topEntry.target.id);
        }
      };
      this.observer = new IntersectionObserver(callback, {
        rootMargin: "-20% 0px -60% 0px",
        threshold: [0, 1]
      });
      this.headings.forEach((heading) => {
        this.observer.observe(heading);
      });
    }
    setActive(id) {
      if (!id) return;
      this.activeParam = id;
      const hash = "#" + id;
      let activeText = "";
      this.tocLinks.forEach((link) => {
        const parentLi = link.closest("li");
        let linkHash = "";
        try {
          const urlList = link.href.split("#");
          linkHash = decodeURIComponent(urlList[urlList.length - 1]);
        } catch (e) {
        }
        if (linkHash === decodeURIComponent(id)) {
          link.classList.add("active", "font-medium");
          if (parentLi) parentLi.classList.add("active");
          activeText = link.textContent;
        } else {
          link.classList.remove("active", "font-medium");
          if (parentLi) parentLi.classList.remove("active");
        }
      });
      if (activeText) {
        if (this.centerTitle) this.centerTitle.textContent = activeText;
      }
      if (this.position === "side") {
        this.updateSideBlocks();
      }
    }
    generateSideBlocks() {
      if (!this.sideIndicator) return;
      this.sideIndicator.innerHTML = "";
      this.tocLinks.forEach((link) => {
        let depth = 0;
        let el = link.parentElement;
        while (el && el.id !== "TableOfContents") {
          if (el.tagName && el.tagName.toLowerCase() === "ul") {
            depth++;
          }
          el = el.parentElement;
        }
        const block = document.createElement("div");
        let widthClass = "w-4";
        if (depth === 2) widthClass = "w-3";
        else if (depth === 3) widthClass = "w-2";
        else if (depth >= 4) widthClass = "w-1";
        block.className = `h-1 ${widthClass} rounded-l-full rounded-r-none transition-all duration-300 bg-muted/50 block-item hover:bg-primary/60`;
        let blockHash = "";
        try {
          const urlList = link.href.split("#");
          blockHash = urlList[urlList.length - 1];
        } catch (e) {
          blockHash = link.hash.substring(1);
        }
        block.dataset.hash = blockHash;
        block.addEventListener("click", (e) => {
          if (!window.matchMedia("(hover: hover)").matches) {
            return;
          }
          e.stopPropagation();
          this.scrollToTarget(link.hash);
        });
        this.sideIndicator.appendChild(block);
      });
    }
    updateSideBlocks() {
      if (!this.sideIndicator) return;
      const blocks = this.sideIndicator.querySelectorAll(".block-item");
      blocks.forEach((block) => {
        let blockHash = block.dataset.hash;
        try {
          blockHash = decodeURIComponent(blockHash);
        } catch (e) {
        }
        if (blockHash === decodeURIComponent(this.activeParam)) {
          block.classList.remove("bg-muted/50");
          block.classList.add("bg-primary", "scale-x-110", "origin-right");
        } else {
          block.classList.remove("bg-primary", "scale-x-110", "origin-right");
          block.classList.add("bg-muted/50");
        }
      });
    }
    scrollToTarget(hash) {
      let targetId = hash.substring(1);
      try {
        targetId = decodeURIComponent(targetId);
      } catch (e) {
      }
      const target = document.getElementById(targetId);
      if (target) {
        const offsetTop = target.getBoundingClientRect().top + window.pageYOffset - 100;
        window.scrollTo({
          top: offsetTop,
          behavior: "smooth"
        });
        this.closeAll();
      }
    }
    bindEvents() {
      const toggleDropdown = (parent, dropdown) => {
        if (!dropdown) return;
        const isHidden = dropdown.classList.contains("hidden");
        if (isHidden) {
          dropdown.classList.remove("hidden");
          void dropdown.offsetWidth;
          dropdown.classList.remove("opacity-0", "scale-95");
          dropdown.classList.add("opacity-100", "scale-100");
        } else {
          dropdown.classList.add("opacity-0", "scale-95");
          dropdown.classList.remove("opacity-100", "scale-100");
          setTimeout(() => {
            dropdown.classList.add("hidden");
          }, 300);
        }
      };
      const closeDropdown = (parent, dropdown) => {
        if (!dropdown) return;
        const isHidden = dropdown.classList.contains("hidden");
        if (!isHidden) {
          dropdown.classList.add("opacity-0", "scale-95");
          dropdown.classList.remove("opacity-100", "scale-100");
          dropdown.classList.add("hidden");
        }
      };
      if (this.centerToggle) {
        let hideTimeout = null;
        this.centerToggle.addEventListener("click", (e) => {
          e.stopPropagation();
          if (this.sectionCenterToggle) {
            closeDropdown(this.sectionCenterToggle.parentElement, this.sectionCenterDropdown);
          }
          toggleDropdown(this.centerToggle.parentElement, this.centerDropdown);
        });
        this.centerToggle.parentElement.addEventListener("mouseenter", () => {
          if (window.matchMedia("(hover: hover)").matches) {
            clearTimeout(hideTimeout);
            if (this.centerDropdown.classList.contains("hidden")) {
              if (this.sectionCenterToggle) {
                closeDropdown(this.sectionCenterToggle.parentElement, this.sectionCenterDropdown);
              }
              toggleDropdown(this.centerToggle.parentElement, this.centerDropdown);
            }
          }
        });
        this.centerToggle.parentElement.addEventListener("mouseleave", () => {
          if (window.matchMedia("(hover: hover)").matches) {
            hideTimeout = setTimeout(() => {
              if (!this.centerDropdown.classList.contains("hidden")) {
                if (this.sectionCenterToggle) {
                  closeDropdown(this.sectionCenterToggle.parentElement, this.sectionCenterDropdown);
                }
                toggleDropdown(this.centerToggle.parentElement, this.centerDropdown);
              }
            }, 200);
          }
        });
      }
      if (this.sectionCenterToggle) {
        let sectionHideTimeout = null;
        this.sectionCenterToggle.addEventListener("click", (e) => {
          e.stopPropagation();
          if (this.centerToggle) {
            closeDropdown(this.centerToggle.parentElement, this.centerDropdown);
          }
          toggleDropdown(this.sectionCenterToggle.parentElement, this.sectionCenterDropdown);
        });
        this.sectionCenterToggle.parentElement.addEventListener("mouseenter", () => {
          if (window.matchMedia("(hover: hover)").matches) {
            clearTimeout(sectionHideTimeout);
            if (this.sectionCenterDropdown.classList.contains("hidden")) {
              if (this.centerToggle) {
                closeDropdown(this.centerToggle.parentElement, this.centerDropdown);
              }
              toggleDropdown(this.sectionCenterToggle.parentElement, this.sectionCenterDropdown);
            }
          }
        });
        this.sectionCenterToggle.parentElement.addEventListener("mouseleave", () => {
          if (window.matchMedia("(hover: hover)").matches) {
            sectionHideTimeout = setTimeout(() => {
              if (!this.sectionCenterDropdown.classList.contains("hidden")) {
                if (this.centerToggle) {
                  closeDropdown(this.centerToggle.parentElement, this.centerDropdown);
                }
                toggleDropdown(this.sectionCenterToggle.parentElement, this.sectionCenterDropdown);
              }
            }, 200);
          }
        });
      }
      if (this.sideIndicator && this.sideDropdown) {
        const container = this.sideIndicator.parentElement;
        let sideHideTimeout = null;
        const toggleSide = (forceHoverActive) => {
          if (forceHoverActive) {
            this.sideDropdown.classList.add("is-open");
          } else {
            this.sideDropdown.classList.remove("is-open");
          }
        };
        container.addEventListener("mouseenter", () => {
          if (window.matchMedia("(hover: hover)").matches) {
            clearTimeout(sideHideTimeout);
            toggleSide(true);
          }
        });
        container.addEventListener("mouseleave", () => {
          if (window.matchMedia("(hover: hover)").matches) {
            sideHideTimeout = setTimeout(() => {
              toggleSide(false);
            }, 200);
          }
        });
        this.sideIndicator.addEventListener("click", (e) => {
          if (!window.matchMedia("(hover: hover)").matches) {
            e.stopPropagation();
            this.sideCloseBtn?.classList.add("is-touch");
            this.sideDropdown.classList.toggle("is-open");
          }
        });
      }
      if (this.sideCloseBtn) {
        this.sideCloseBtn.addEventListener("click", (e) => {
          e.stopPropagation();
          if (this.sideDropdown) {
            this.sideDropdown.classList.remove("is-open");
          }
        });
      }
      document.addEventListener("toc:toggle", () => {
        if (this.position === "center" && this.centerToggle) {
          this.centerToggle.click();
        }
      });
      document.addEventListener("click", (e) => {
        let clickedInside = false;
        if (this.centerDropdown && this.centerToggle) {
          if (this.centerToggle.parentElement.contains(e.target)) clickedInside = true;
        }
        if (this.sectionCenterDropdown && this.sectionCenterToggle) {
          if (this.sectionCenterToggle.parentElement.contains(e.target)) clickedInside = true;
        }
        if (this.sideIndicator && this.sideDropdown) {
          if (this.sideIndicator.parentElement.contains(e.target) || this.sideDropdown.contains(e.target)) clickedInside = true;
        }
        if (!clickedInside) {
          this.closeAll();
        }
      });
      this.container.addEventListener("click", (e) => {
        const link = e.target.closest("a");
        if (link && link.hash) {
          e.preventDefault();
          this.scrollToTarget(link.hash);
        }
      });
    }
    closeAll() {
      [this.centerDropdown, this.sectionCenterDropdown].forEach((dropdown) => {
        if (dropdown && !dropdown.classList.contains("hidden")) {
          dropdown.classList.add("opacity-0", "scale-95");
          dropdown.classList.remove("opacity-100", "scale-100");
          setTimeout(() => {
            dropdown.classList.add("hidden");
          }, 300);
        }
      });
      if (this.sideDropdown && this.sideDropdown.classList.contains("is-open")) {
        this.sideDropdown.classList.remove("is-open");
      }
    }
    exposeAPI() {
      window.TOC = {
        toggle: () => {
          if ((this.position === "center" || this.position === "left") && this.centerToggle) {
            this.centerToggle.click();
          }
        },
        hide: () => this.closeAll(),
        initialized: true
      };
    }
  };
  function initTOC() {
    if (window.tocManagerInstance) return;
    if (document.readyState === "loading") {
      document.addEventListener("DOMContentLoaded", () => {
        if (!window.tocManagerInstance) window.tocManagerInstance = new TOCManager();
      });
    } else {
      setTimeout(() => {
        if (!window.tocManagerInstance) window.tocManagerInstance = new TOCManager();
      }, 50);
    }
  }
  document.addEventListener("pjax:complete", () => {
    window.tocManagerInstance = null;
    initTOC();
  });
  initTOC();
})();
