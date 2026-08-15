(() => {
  // ns-hugo-imp:E:\workspace\notebook\hugo\cpp_notes\themes\hugo-narrow\assets\js\clipboard.js
  async function copyText(text) {
    if (!navigator.clipboard?.writeText || !window.isSecureContext) {
      return false;
    }
    try {
      await navigator.clipboard.writeText(text);
      return true;
    } catch (_) {
      return false;
    }
  }

  // ns-hugo-imp:E:\workspace\notebook\hugo\cpp_notes\themes\hugo-narrow\assets\js\codeblock.js
  var COPY_FEEDBACK_MS = 2e3;
  var copyTimers = /* @__PURE__ */ new WeakMap();
  function getCodeBlocks(root = document) {
    return [...root.querySelectorAll("[data-code-block]")];
  }
  function countSourceLines(block) {
    const sourceNode = block.querySelector("[data-code-source]");
    const sourceText = sourceNode?.textContent ?? "";
    if (!sourceText) return 0;
    return sourceText.split("\n").length;
  }
  function shouldAutoCollapse(block) {
    const autoCollapseLines = Number.parseInt(block.dataset.autoCollapseLines || "30", 10);
    const autoCollapseHeight = Number.parseInt(block.dataset.autoCollapseHeight || "400", 10);
    const renderedCode = block.querySelector(
      ".code-block-content pre.chroma, .code-block-content pre"
    );
    const renderedHeight = renderedCode?.offsetHeight || 0;
    const sourceLines = countSourceLines(block);
    return sourceLines > autoCollapseLines || renderedHeight > autoCollapseHeight;
  }
  function updateCollapseButton(block, collapsed) {
    const button = block.querySelector(".collapse-code-btn");
    if (!button) return;
    const label = collapsed ? button.dataset.labelExpand : button.dataset.labelCollapse;
    const text = button.querySelector(".collapse-text");
    const chevron = button.querySelector(".collapse-chevron");
    if (text) text.textContent = label || "";
    if (chevron) chevron.classList.toggle("rotate-180", !collapsed);
    button.title = label || "";
    button.setAttribute("aria-label", label || "");
    button.setAttribute("aria-expanded", String(!collapsed));
  }
  function updateCollapseOverlay(block, collapsed) {
    const overlay = block.querySelector(".collapse-overlay");
    if (!overlay) return;
    overlay.hidden = !collapsed;
    overlay.classList.toggle("pointer-events-none", !collapsed);
    overlay.classList.toggle("opacity-0", !collapsed);
    overlay.classList.toggle("pointer-events-auto", collapsed);
    overlay.classList.toggle("opacity-100", collapsed);
  }
  function setCollapsed(block, collapsed, { animate = true } = {}) {
    const content = block.querySelector(".code-block-content");
    if (!content) return;
    const collapsedHeight = Number.parseInt(block.dataset.collapsedHeight || "120", 10);
    block.dataset.collapsed = String(collapsed);
    content.style.transition = animate ? "max-height 0.3s ease-out" : "";
    content.style.maxHeight = collapsed ? `${collapsedHeight}px` : "";
    content.style.overflow = collapsed ? "hidden" : "";
    updateCollapseButton(block, collapsed);
    updateCollapseOverlay(block, collapsed);
    if (animate) {
      window.setTimeout(() => {
        content.style.transition = "";
      }, 300);
    }
  }
  function initCollapsibleBlocks(root = document) {
    getCodeBlocks(root).forEach((block) => {
      if (block.dataset.collapsible !== "true") return;
      const defaultState = block.dataset.defaultState || "expanded";
      const collapsedAttr = block.dataset.collapsed === "true";
      const startCollapsed = collapsedAttr || defaultState === "collapsed" || shouldAutoCollapse(block);
      setCollapsed(block, startCollapsed, { animate: false });
    });
  }
  function getCodeSource(block) {
    const sourceNode = block.querySelector("[data-code-source]");
    return sourceNode?.textContent ?? "";
  }
  function setCopyFeedback(button, copied) {
    const text = button.querySelector(".copy-text");
    const label = copied ? button.dataset.labelCopied : button.dataset.labelCopy;
    if (text) text.textContent = label || "";
    button.classList.toggle("text-green-600", copied);
    button.title = label || "";
    button.setAttribute("aria-label", label || "");
  }
  async function handleCopy(button) {
    const block = button.closest("[data-code-block]");
    if (!block) return;
    const copied = await copyText(getCodeSource(block));
    if (!copied) return;
    setCopyFeedback(button, true);
    const previousTimer = copyTimers.get(button);
    if (previousTimer) {
      window.clearTimeout(previousTimer);
    }
    const timer = window.setTimeout(() => {
      setCopyFeedback(button, false);
      copyTimers.delete(button);
    }, COPY_FEEDBACK_MS);
    copyTimers.set(button, timer);
  }
  function handleCodeBlockClick(event) {
    const copyButton = event.target.closest(".copy-code-btn[data-code-action='copy']");
    if (copyButton) {
      event.preventDefault();
      handleCopy(copyButton);
      return;
    }
    const collapseButton = event.target.closest(
      ".collapse-code-btn[data-code-action='toggle-collapse']"
    );
    if (collapseButton) {
      event.preventDefault();
      const block = collapseButton.closest("[data-code-block]");
      if (!block) return;
      const isCollapsed = block.dataset.collapsed === "true";
      setCollapsed(block, !isCollapsed);
      return;
    }
    const expandButton = event.target.closest(".collapse-overlay-btn[data-code-action='expand']");
    if (expandButton) {
      event.preventDefault();
      const block = expandButton.closest("[data-code-block]");
      if (!block) return;
      setCollapsed(block, false);
    }
  }
  var initialized = false;
  function initCodeBlocks(root = document) {
    if (initialized) return;
    initialized = true;
    initCollapsibleBlocks(root);
    document.addEventListener("click", handleCodeBlockClick);
  }

  // ns-hugo-imp:E:\workspace\notebook\hugo\cpp_notes\themes\hugo-narrow\assets\js\tabs.js
  var NAV_SHELL_CLASS = "tabs-nav-shell overflow-x-auto px-2 pt-2 pb-1 sm:px-3 sm:pt-3 [-ms-overflow-style:none] [scrollbar-width:none] [&::-webkit-scrollbar]:hidden";
  var NAV_CLASS = "tabs-nav inline-flex min-w-max items-center gap-1 rounded-xl border border-border bg-background/70 p-1 shadow-sm text-muted-foreground";
  var TRIGGER_BASE_CLASS = "tabs-trigger inline-flex items-center justify-center whitespace-nowrap rounded-lg px-3.5 py-1.5 text-sm font-medium transition-[color,background-color,box-shadow] duration-200 ease-out focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-primary/20";
  var TRIGGER_ACTIVE_CLASS = "bg-primary/10 text-primary shadow-xs";
  var TRIGGER_INACTIVE_CLASS = "text-muted-foreground hover:bg-primary/10 hover:text-primary";
  function getTabWrappers(root = document) {
    return [...root.querySelectorAll("[data-tabs]")];
  }
  function getTabButtons(wrapper) {
    return [...wrapper.querySelectorAll(".tabs-trigger[role='tab']")];
  }
  function getTabPanels(wrapper) {
    return [...wrapper.querySelectorAll(".tabs-panel[data-tabs-panel][role='tabpanel']")];
  }
  function getPanelsContainer(wrapper) {
    return wrapper.querySelector(".tabs-panels");
  }
  function buildTabNav(wrapper) {
    const panels = getTabPanels(wrapper);
    if (panels.length === 0) return [];
    wrapper.querySelector(".tabs-nav-shell")?.remove();
    const navShell = document.createElement("div");
    navShell.className = NAV_SHELL_CLASS;
    const nav = document.createElement("div");
    nav.className = NAV_CLASS;
    nav.setAttribute("role", "tablist");
    const wrapperId = wrapper.id || `tabs-${Math.random().toString(36).slice(2, 8)}`;
    wrapper.id = wrapperId;
    panels.forEach((panel, index) => {
      const panelId = panel.id || `${wrapperId}-panel-${index}`;
      const buttonId = `${wrapperId}-tab-${index}`;
      const label = panel.dataset.tabLabel || `Tab ${index + 1}`;
      panel.id = panelId;
      panel.setAttribute("aria-labelledby", buttonId);
      const button = document.createElement("button");
      button.id = buttonId;
      button.className = `${TRIGGER_BASE_CLASS} ${TRIGGER_INACTIVE_CLASS}`;
      button.type = "button";
      button.setAttribute("role", "tab");
      button.dataset.tabTarget = panelId;
      button.setAttribute("aria-controls", panelId);
      button.setAttribute("aria-selected", "false");
      button.tabIndex = -1;
      button.textContent = label;
      nav.append(button);
    });
    navShell.append(nav);
    const panelsContainer = getPanelsContainer(wrapper);
    if (panelsContainer) {
      panelsContainer.before(navShell);
    } else {
      wrapper.prepend(navShell);
    }
    return getTabButtons(wrapper);
  }
  function updateTriggerState(button, isActive) {
    button.classList.toggle("active", isActive);
    TRIGGER_ACTIVE_CLASS.split(" ").forEach((className) => {
      button.classList.toggle(className, isActive);
    });
    TRIGGER_INACTIVE_CLASS.split(" ").forEach((className) => {
      button.classList.toggle(className, !isActive);
    });
  }
  function activateTab(button) {
    const wrapper = button.closest("[data-tabs]");
    if (!wrapper) return;
    const targetId = button.dataset.tabTarget;
    const targetPanel = wrapper.querySelector(`#${CSS.escape(targetId)}`);
    if (!targetPanel) return;
    getTabButtons(wrapper).forEach((tabButton) => {
      const isActive = tabButton === button;
      updateTriggerState(tabButton, isActive);
      tabButton.setAttribute("aria-selected", String(isActive));
      tabButton.tabIndex = isActive ? 0 : -1;
    });
    getTabPanels(wrapper).forEach((panel) => {
      const isActive = panel === targetPanel;
      panel.hidden = !isActive;
    });
  }
  function focusAdjacentTab(currentButton, direction) {
    const wrapper = currentButton.closest("[data-tabs]");
    if (!wrapper) return;
    const buttons = getTabButtons(wrapper);
    const currentIndex = buttons.indexOf(currentButton);
    if (currentIndex === -1) return;
    const nextIndex = (currentIndex + direction + buttons.length) % buttons.length;
    const nextButton = buttons[nextIndex];
    if (!nextButton) return;
    nextButton.focus();
    activateTab(nextButton);
  }
  function handleClick(event) {
    const tabButton = event.target.closest(".tabs-trigger[role='tab']");
    if (!tabButton) return;
    event.preventDefault();
    activateTab(tabButton);
  }
  function handleKeydown(event) {
    const button = event.target.closest(".tabs-trigger[role='tab']");
    if (!button) return;
    if (event.key === "ArrowRight") {
      event.preventDefault();
      focusAdjacentTab(button, 1);
      return;
    }
    if (event.key === "ArrowLeft") {
      event.preventDefault();
      focusAdjacentTab(button, -1);
      return;
    }
    if (event.key === "Home") {
      event.preventDefault();
      const wrapper = button.closest("[data-tabs]");
      const firstButton = wrapper ? getTabButtons(wrapper)[0] : null;
      if (!firstButton) return;
      firstButton.focus();
      activateTab(firstButton);
      return;
    }
    if (event.key === "End") {
      event.preventDefault();
      const wrapper = button.closest("[data-tabs]");
      const buttons = wrapper ? getTabButtons(wrapper) : [];
      const lastButton = buttons[buttons.length - 1];
      if (!lastButton) return;
      lastButton.focus();
      activateTab(lastButton);
    }
  }
  var initialized2 = false;
  function initTabs(root = document) {
    getTabWrappers(root).forEach((wrapper) => {
      const buttons = buildTabNav(wrapper);
      const panels = getTabPanels(wrapper);
      if (buttons.length === 0 || panels.length === 0) return;
      const activeButton = buttons.find((button) => button.getAttribute("aria-selected") === "true") || buttons[0];
      activateTab(activeButton);
    });
    if (initialized2) return;
    initialized2 = true;
    document.addEventListener("click", handleClick);
    document.addEventListener("keydown", handleKeydown);
  }

  // ns-hugo-imp:E:\workspace\notebook\hugo\cpp_notes\themes\hugo-narrow\assets\js\ui.js
  var ToolDropdown = class {
    constructor(uiManager2) {
      this.ui = uiManager2;
      this.types = ["color-scheme", "theme", "language", "content-width"];
      this.originalPositions = /* @__PURE__ */ new WeakMap();
      this.setup();
    }
    setup() {
      this.types.forEach((type) => this.bindToggle(type));
    }
    bindToggle(type) {
      const toggleSelector = `.dropdown-toggle[data-dropdown-type="${type}"]`;
      const dropdownSelector = `.dropdown-menu[data-dropdown-type="${type}"]`;
      document.querySelectorAll(toggleSelector).forEach((toggle) => {
        const wrapper = toggle.closest(".relative") || toggle.parentElement;
        const dropdown = wrapper?.querySelector(dropdownSelector);
        if (!toggle || !dropdown) return;
        toggle.addEventListener("click", (event) => {
          event.stopPropagation();
          this.ui.navDisclosure.closePanel({ restoreFocus: false });
          const isHidden = dropdown.classList.contains("hidden");
          this.closeAll(toggle, dropdown);
          if (isHidden) {
            dropdown.classList.remove("hidden");
            if (type === "content-width") {
              this.freezePosition(dropdown);
            }
            toggle.setAttribute("aria-expanded", "true");
            return;
          }
          dropdown.classList.add("hidden");
          this.releasePosition(dropdown);
          toggle.setAttribute("aria-expanded", "false");
        });
      });
    }
    freezePosition(dropdown) {
      const rect = dropdown.getBoundingClientRect();
      this.originalPositions.set(dropdown, {
        parent: dropdown.parentNode,
        nextSibling: dropdown.nextSibling
      });
      document.body.append(dropdown);
      Object.assign(dropdown.style, {
        position: "fixed",
        top: `${rect.top}px`,
        right: "auto",
        bottom: "auto",
        left: `${rect.left}px`,
        width: `${rect.width}px`
      });
    }
    releasePosition(dropdown) {
      dropdown.removeAttribute("style");
      const position = this.originalPositions.get(dropdown);
      if (!position) return;
      position.parent.insertBefore(dropdown, position.nextSibling);
      this.originalPositions.delete(dropdown);
    }
    closeAll(exceptToggle = null, exceptDropdown = null) {
      this.types.forEach((type) => {
        document.querySelectorAll(`.dropdown-menu[data-dropdown-type="${type}"]`).forEach((dropdown) => {
          if (dropdown === exceptDropdown) return;
          dropdown.classList.add("hidden");
          this.releasePosition(dropdown);
        });
        document.querySelectorAll(`.dropdown-toggle[data-dropdown-type="${type}"]`).forEach((toggle) => {
          if (toggle === exceptToggle) return;
          toggle.setAttribute("aria-expanded", "false");
        });
      });
    }
  };
  var NavDisclosure = class {
    constructor(uiManager2) {
      this.ui = uiManager2;
      this.panel = document.getElementById("mobile-nav-panel");
      this.toggle = document.getElementById("mobile-nav-toggle");
      this.lastFocusedElement = null;
      this.setup();
    }
    setup() {
      this.setupPanelToggle();
      this.setupAccordions();
      this.setupDesktopSubmenus();
      this.setupPanelLinkClose();
      this.setupPanelKeyboard();
    }
    setupPanelToggle() {
      if (!this.toggle || !this.panel) return;
      this.toggle.addEventListener("click", (event) => {
        event.stopPropagation();
        this.ui.toolDropdown.closeAll();
        if (this.panel.classList.contains("hidden")) {
          this.openPanel();
        } else {
          this.closePanel();
        }
      });
    }
    openPanel() {
      if (!this.panel || !this.toggle) return;
      this.lastFocusedElement = document.activeElement instanceof HTMLElement ? document.activeElement : null;
      this.panel.classList.remove("hidden");
      this.panel.setAttribute("aria-hidden", "false");
      this.toggle.setAttribute("aria-expanded", "true");
      this.ui.lockScroll("mobile-nav");
      this.focusFirstElement();
    }
    closePanel({ restoreFocus = true } = {}) {
      if (!this.panel) return;
      this.panel.classList.add("hidden");
      this.panel.setAttribute("aria-hidden", "true");
      if (this.toggle) {
        this.toggle.setAttribute("aria-expanded", "false");
      }
      this.closeAllAccordions();
      this.ui.unlockScroll("mobile-nav");
      if (restoreFocus && this.lastFocusedElement instanceof HTMLElement) {
        this.lastFocusedElement.focus();
      }
      this.lastFocusedElement = null;
    }
    setupAccordions() {
      document.querySelectorAll(".nav-accordion-toggle").forEach((toggle) => {
        toggle.addEventListener("click", (event) => {
          event.stopPropagation();
          const id = toggle.getAttribute("data-accordion-id");
          const panel = document.querySelector(`.nav-accordion-panel[data-accordion-id="${id}"]`);
          if (!panel) return;
          const isOpen = toggle.getAttribute("aria-expanded") === "true";
          this.closeAllAccordions();
          if (!isOpen) {
            panel.style.gridTemplateRows = "1fr";
            panel.setAttribute("aria-hidden", "false");
            toggle.setAttribute("aria-expanded", "true");
            toggle.querySelector(".accordion-chevron")?.classList.add("rotate-180");
            toggle.classList.add("bg-primary/10", "text-primary");
          }
        });
      });
    }
    closeAllAccordions() {
      document.querySelectorAll(".nav-accordion-panel").forEach((panel) => {
        panel.style.gridTemplateRows = "0fr";
        panel.setAttribute("aria-hidden", "true");
      });
      document.querySelectorAll(".nav-accordion-toggle").forEach((toggle) => {
        toggle.setAttribute("aria-expanded", "false");
        toggle.querySelector(".accordion-chevron")?.classList.remove("rotate-180");
        toggle.classList.remove("bg-primary/10", "text-primary");
      });
    }
    setupDesktopSubmenus() {
      document.querySelectorAll(".nav-submenu-toggle").forEach((toggle) => {
        toggle.addEventListener("click", (event) => {
          event.stopPropagation();
          const id = toggle.getAttribute("data-submenu-id");
          const submenu = document.querySelector(`.nav-submenu[data-submenu-id="${id}"]`);
          if (!submenu) return;
          const isOpen = !submenu.classList.contains("hidden");
          this.ui.toolDropdown.closeAll();
          this.closeAllDesktopSubmenus();
          if (!isOpen) {
            submenu.classList.remove("hidden");
            toggle.setAttribute("aria-expanded", "true");
            toggle.querySelector(".submenu-chevron")?.classList.add("rotate-180");
          }
        });
      });
    }
    closeAllDesktopSubmenus() {
      document.querySelectorAll(".nav-submenu").forEach((submenu) => {
        submenu.classList.add("hidden");
        const id = submenu.getAttribute("data-submenu-id");
        const toggle = document.querySelector(`.nav-submenu-toggle[data-submenu-id="${id}"]`);
        if (!toggle) return;
        toggle.setAttribute("aria-expanded", "false");
        toggle.querySelector(".submenu-chevron")?.classList.remove("rotate-180");
      });
    }
    setupPanelLinkClose() {
      if (!this.panel) return;
      this.panel.addEventListener("click", (event) => {
        if (!event.target.closest("a[href]")) return;
        setTimeout(() => this.closePanel({ restoreFocus: false }), 100);
      });
    }
    setupPanelKeyboard() {
      document.addEventListener("keydown", (event) => {
        if (!this.isPanelOpen() || event.key !== "Tab") return;
        this.trapFocus(event);
      });
    }
    isPanelOpen() {
      return Boolean(this.panel) && !this.panel.classList.contains("hidden");
    }
    getFocusableElements() {
      if (!this.panel) return [];
      const focusableSelector = [
        "a[href]",
        "button:not([disabled])",
        "input:not([disabled])",
        "select:not([disabled])",
        "textarea:not([disabled])",
        "[tabindex]:not([tabindex='-1'])"
      ].join(", ");
      return [...this.panel.querySelectorAll(focusableSelector)].filter((element) => {
        if (!(element instanceof HTMLElement) || element.hasAttribute("hidden")) {
          return false;
        }
        return !element.closest('.nav-accordion-panel[aria-hidden="true"]');
      });
    }
    focusFirstElement() {
      const [firstFocusable] = this.getFocusableElements();
      if (firstFocusable) {
        firstFocusable.focus();
        return;
      }
      this.panel?.focus();
    }
    trapFocus(event) {
      const focusableElements = this.getFocusableElements();
      if (focusableElements.length === 0) {
        event.preventDefault();
        this.panel?.focus();
        return;
      }
      const firstElement = focusableElements[0];
      const lastElement = focusableElements[focusableElements.length - 1];
      const activeElement = document.activeElement;
      if (!this.panel?.contains(activeElement)) {
        event.preventDefault();
        (event.shiftKey ? lastElement : firstElement).focus();
        return;
      }
      if (event.shiftKey && activeElement === firstElement) {
        event.preventDefault();
        lastElement.focus();
        return;
      }
      if (!event.shiftKey && activeElement === lastElement) {
        event.preventDefault();
        firstElement.focus();
      }
    }
    closeAll(options) {
      this.closePanel(options);
      this.closeAllDesktopSubmenus();
    }
  };
  var UIManager = class {
    constructor() {
      this.theme = localStorage.getItem("theme") || "system";
      this.colorScheme = localStorage.getItem("colorScheme") || document.documentElement.getAttribute("data-theme") || "default";
      this.scrollLocks = /* @__PURE__ */ new Set();
      this.previousBodyOverflow = "";
      this.init();
    }
    init() {
      this.navDisclosure = new NavDisclosure(this);
      this.toolDropdown = new ToolDropdown(this);
      this.exposeAPI();
      this.setupGlobalListeners();
      this.setupContentWidthControl();
      this.updateUI();
    }
    closeAllMenus(options = {}) {
      this.toolDropdown.closeAll();
      this.navDisclosure.closeAll(options);
    }
    exposeAPI() {
      window.HugoNarrowUI = {
        closeAllMenus: (options = {}) => this.closeAllMenus(options),
        lockScroll: (key) => this.lockScroll(key),
        unlockScroll: (key) => this.unlockScroll(key)
      };
    }
    setupGlobalListeners() {
      document.addEventListener("click", (event) => {
        const colorSchemeButton = event.target.closest(
          '.dropdown-menu[data-dropdown-type="color-scheme"] [data-color-scheme]'
        );
        if (colorSchemeButton) {
          this.setColorScheme(colorSchemeButton.getAttribute("data-color-scheme"));
          this.closeAllMenus();
          return;
        }
        const themeButton = event.target.closest(
          '.dropdown-menu[data-dropdown-type="theme"] [data-theme]'
        );
        if (themeButton) {
          this.setTheme(themeButton.getAttribute("data-theme"));
          this.closeAllMenus();
          return;
        }
        const isInside = event.target.closest(
          ".dropdown-toggle, .dropdown-menu, .nav-submenu-toggle, .nav-submenu, .nav-panel-toggle, #mobile-nav-panel"
        );
        if (!isInside) {
          this.closeAllMenus();
        }
      });
      document.addEventListener("keydown", (event) => {
        if (event.key === "Escape") {
          this.closeAllMenus({ restoreFocus: true });
        }
      });
      window.matchMedia("(prefers-color-scheme: dark)").addEventListener("change", () => {
        if (this.theme !== "system") return;
        this.applyTheme();
        this.updateUI();
      });
      window.addEventListener("resize", () => this.closeAllMenus());
    }
    setupContentWidthControl() {
      const container = document.getElementById("page-container");
      const range = document.getElementById("content-width-range");
      const output = document.getElementById("content-width-value");
      const reset = document.getElementById("content-width-reset");
      if (!container || !range || !output || !reset) return;
      const min = Number(range.min);
      const max = Number(range.max);
      const clamp = (value) => Math.min(max, Math.max(min, Math.round(value)));
      const storedWidth = Number.parseInt(localStorage.getItem("contentWidth"), 10);
      const rootFontSize = Number.parseFloat(getComputedStyle(document.documentElement).fontSize) || 16;
      const defaultWidth = Number.parseFloat(getComputedStyle(container).maxWidth) / rootFontSize;
      const hasStoredWidth = Number.isInteger(storedWidth) && storedWidth >= min && storedWidth <= max;
      const initialWidth = hasStoredWidth ? storedWidth : defaultWidth;
      const updateControl = (value) => {
        const width = clamp(value);
        range.value = String(width);
        output.value = `${width}rem`;
        output.textContent = `${width}rem`;
        return width;
      };
      updateControl(initialWidth);
      let updateFrame = null;
      let pendingWidth = initialWidth;
      range.addEventListener("input", () => {
        const width = updateControl(Number(range.value));
        pendingWidth = width;
        if (updateFrame !== null) return;
        updateFrame = requestAnimationFrame(() => {
          document.documentElement.style.setProperty("--content-width", `${pendingWidth}rem`);
          localStorage.setItem("contentWidth", String(pendingWidth));
          updateFrame = null;
        });
      });
      reset.addEventListener("click", () => {
        if (updateFrame !== null) {
          cancelAnimationFrame(updateFrame);
          updateFrame = null;
        }
        localStorage.removeItem("contentWidth");
        document.documentElement.style.removeProperty("--content-width");
        const configuredWidth = Number.parseFloat(getComputedStyle(container).maxWidth) / rootFontSize;
        updateControl(configuredWidth);
      });
    }
    emitThemeChanged() {
      const detail = { colorScheme: this.colorScheme, theme: this.theme };
      window.dispatchEvent(new CustomEvent("theme:changed", { detail }));
      window.dispatchEvent(new CustomEvent("themeChanged", { detail }));
    }
    setColorScheme(colorScheme) {
      this.colorScheme = colorScheme;
      localStorage.setItem("colorScheme", colorScheme);
      document.documentElement.setAttribute("data-theme", colorScheme);
      this.updateUI();
      this.emitThemeChanged();
    }
    setTheme(theme) {
      this.theme = theme;
      localStorage.setItem("theme", theme);
      this.applyTheme();
      this.updateUI();
      this.emitThemeChanged();
    }
    applyTheme() {
      const isDark = this.theme === "dark" || this.theme === "system" && window.matchMedia("(prefers-color-scheme: dark)").matches;
      document.documentElement.classList.toggle("dark", isDark);
    }
    lockScroll(key = "default") {
      if (this.scrollLocks.has(key)) return;
      if (this.scrollLocks.size === 0) {
        this.previousBodyOverflow = document.body.style.overflow;
        document.body.style.overflow = "hidden";
      }
      this.scrollLocks.add(key);
    }
    unlockScroll(key = "default") {
      if (!this.scrollLocks.has(key)) return;
      this.scrollLocks.delete(key);
      if (this.scrollLocks.size === 0) {
        document.body.style.overflow = this.previousBodyOverflow;
      }
    }
    updateUI() {
      const sunIcons = document.querySelectorAll(".sun-icon, #sun-icon");
      const moonIcons = document.querySelectorAll(".moon-icon, #moon-icon");
      const systemIcons = document.querySelectorAll(".system-icon, #system-icon");
      [...sunIcons, ...moonIcons, ...systemIcons].forEach((icon) => {
        icon.classList.add("hidden");
      });
      if (this.theme === "light") {
        sunIcons.forEach((icon) => icon.classList.remove("hidden"));
      } else if (this.theme === "dark") {
        moonIcons.forEach((icon) => icon.classList.remove("hidden"));
      } else {
        systemIcons.forEach((icon) => icon.classList.remove("hidden"));
      }
      this.updateDropdownSelection();
    }
    updateDropdownSelection() {
      const selectedClasses = ["bg-accent", "text-accent-foreground"];
      document.querySelectorAll("[data-color-scheme]").forEach((button) => {
        const isSelected = button.getAttribute("data-color-scheme") === this.colorScheme;
        selectedClasses.forEach((className) => {
          button.classList.toggle(className, isSelected);
        });
      });
      document.querySelectorAll("[data-theme]").forEach((button) => {
        const isSelected = button.getAttribute("data-theme") === this.theme;
        selectedClasses.forEach((className) => {
          button.classList.toggle(className, isSelected);
        });
      });
      const currentLang = document.documentElement.lang || "en";
      document.querySelectorAll('.dropdown-menu[data-dropdown-type="language"] a[role="menuitem"]').forEach((link) => {
        const href = link.getAttribute("href");
        const isSelected = this.isCurrentLanguageLink(href, currentLang);
        selectedClasses.forEach((className) => {
          link.classList.toggle(className, isSelected);
        });
      });
    }
    isCurrentLanguageLink(href, currentLang) {
      if (href === "/" && currentLang === "en") {
        return true;
      }
      return new RegExp(`^/${currentLang}(/|$)`).test(href);
    }
  };
  var initialized3 = false;
  var uiManager = null;
  function initUI() {
    if (initialized3) return uiManager;
    initialized3 = true;
    uiManager = new UIManager();
    return uiManager;
  }

  // <stdin>
  function boot() {
    initUI();
    initTabs();
    initCodeBlocks();
  }
  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", boot, { once: true });
  } else {
    boot();
  }
})();
