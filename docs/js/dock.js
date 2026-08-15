(() => {
  // <stdin>
  (function() {
    "use strict";
    let lastScrollTop = 0;
    let isScrollingUp = false;
    let scrollThreshold = 100;
    let isDockVisible = false;
    const dock = document.getElementById("dock");
    const floatTrigger = document.getElementById("dock-float-trigger");
    if (!dock) return;
    const dockMode = dock.dataset.dockMode || "scroll";
    function handleScroll() {
      const currentScrollTop = window.pageYOffset || document.documentElement.scrollTop;
      if (currentScrollTop < lastScrollTop) {
        isScrollingUp = true;
      } else {
        isScrollingUp = false;
      }
      switch (dockMode) {
        case "scroll":
          if (isScrollingUp && currentScrollTop > scrollThreshold) {
            showDock();
          } else if (!isScrollingUp || currentScrollTop <= scrollThreshold) {
            hideDock();
          }
          break;
        case "always":
          showDock();
          break;
        case "float":
          if (isDockVisible && currentScrollTop > lastScrollTop) {
            hideDock();
          }
          break;
      }
      lastScrollTop = currentScrollTop;
    }
    function showDock() {
      dock.classList.remove("translate-y-24", "opacity-0", "pointer-events-none");
      dock.classList.add("translate-y-0", "opacity-100", "pointer-events-auto");
      isDockVisible = true;
    }
    function hideDock() {
      dock.classList.remove("translate-y-0", "opacity-100", "pointer-events-auto");
      dock.classList.add("translate-y-24", "opacity-0", "pointer-events-none");
      isDockVisible = false;
    }
    function toggleFloatDock() {
      if (isDockVisible) {
        hideDock();
      } else {
        showDock();
      }
    }
    function throttle(func, limit) {
      let inThrottle;
      return function() {
        const args = arguments;
        const context = this;
        if (!inThrottle) {
          func.apply(context, args);
          inThrottle = true;
          setTimeout(() => inThrottle = false, limit);
        }
      };
    }
    window.addEventListener("scroll", throttle(handleScroll, 16));
    if (floatTrigger && dockMode === "float") {
      let hoverTimer;
      floatTrigger.addEventListener("click", function(e) {
        e.preventDefault();
        toggleFloatDock();
      });
      floatTrigger.addEventListener("mouseenter", function() {
        clearTimeout(hoverTimer);
        hoverTimer = setTimeout(() => {
          if (!isDockVisible) {
            showDock();
          }
        }, 150);
      });
      floatTrigger.addEventListener("mouseleave", function() {
        clearTimeout(hoverTimer);
        setTimeout(() => {
          if (isDockVisible && !dock.matches(":hover")) {
            hideDock();
          }
        }, 200);
      });
      dock.addEventListener("mouseenter", function() {
        clearTimeout(hoverTimer);
      });
      dock.addEventListener("mouseleave", function() {
        setTimeout(() => {
          if (isDockVisible && !floatTrigger.matches(":hover")) {
            hideDock();
          }
        }, 200);
      });
      document.addEventListener("click", function(e) {
        if (isDockVisible && !dock.contains(e.target) && !floatTrigger.contains(e.target)) {
          hideDock();
        }
      });
      dock.addEventListener("click", function(e) {
        e.stopPropagation();
      });
    }
    const backBtn = document.getElementById("dock-back");
    if (backBtn) {
      backBtn.addEventListener("click", function(e) {
        e.preventDefault();
        try {
          if (window.history.length > 1 && document.referrer) {
            const referrerUrl = new URL(document.referrer);
            const currentUrl = new URL(window.location.href);
            if (referrerUrl.origin === currentUrl.origin) {
              window.history.back();
              return;
            }
          }
          window.location.href = "/";
        } catch (error) {
          window.location.href = "/";
        }
      });
    }
    const searchBtnIds = ["dock-search", "header-search"];
    searchBtnIds.forEach(function(id) {
      const btn = document.getElementById(id);
      if (!btn) return;
      btn.addEventListener("click", function(e) {
        e.preventDefault();
        const detail = { origin: "dock", handled: false };
        document.dispatchEvent(new CustomEvent("search:toggle", { detail }));
        if (!detail.handled && window.Search?.toggle) {
          window.Search.toggle();
        }
      });
    });
    const commentsBtn = document.getElementById("dock-comments");
    if (commentsBtn) {
      commentsBtn.addEventListener("click", function(e) {
        e.preventDefault();
        const commentElement = document.getElementById("comments");
        if (commentElement) {
          commentElement.scrollIntoView({
            behavior: "smooth",
            block: "start",
            inline: "nearest"
          });
        } else {
          window.scrollTo({
            top: document.documentElement.scrollHeight,
            behavior: "smooth"
          });
        }
      });
    }
    const topBtn = document.getElementById("dock-top");
    if (topBtn) {
      topBtn.addEventListener("click", function() {
        window.scrollTo({
          top: 0,
          behavior: "smooth"
        });
      });
    }
    switch (dockMode) {
      case "always":
        showDock();
        break;
      case "float":
        hideDock();
        if (floatTrigger) {
          floatTrigger.style.opacity = "1";
        }
        break;
      case "scroll":
      default:
        hideDock();
        break;
    }
  })();
})();
