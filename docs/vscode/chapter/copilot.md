# copilot


# 自定义模型

1. 下载并安装 [insider](https://code.visualstudio.com/insiders/) 版本
2. 切换到 `${install_folder}/Microsoft VS Code Insiders/resources/app/out/vs/workbench` 文件夹

```term
triangle@LEARN:~$ npm install -g prettier
triangle@LEARN:~$ prettier --write workbench.desktop.main.js
```

3. 打开 `workbench.desktop.main.js` 文件，搜索 `manageModels`

```js
  ...
function xVs(i, e) {
  return {
    getActions: () => {
      const s = [];
      return (
        ( e.anonymous ||                      // NOTE - 添加该行判断
          e.entitlement === vs.Free ||
          e.entitlement === vs.Pro ||
          e.entitlement === vs.ProPlus ||
          e.isInternal) &&
          s.push(
            {
              id: "manageModels",
              label: d(5932, null),
              enabled: !0,
              tooltip: d(5933, null),
              class: void 0,
              run: () => {
                const n = upi.ID;
                i.executeCommand(n);
              },
            }
         ),
         (e.anonymous || e.entitlement === vs.Free) &&
          s.push({
            id: "moreModels",
            label: d(5934, null),
            enabled: !0,
            tooltip: d(5935, null),
            class: void 0,
            run: () => {
              const n = e.anonymous
                ? "workbench.action.chat.triggerSetup"
                : "workbench.action.chat.upgradePlan";
              i.executeCommand(n);
            },
          }),
        s
      );
    },
  };
}
  ...
```

4. 打开 `vscode insider` 在 `settings.json` 文件中配置

  ```json
   "chat.allowAnonymousAccess": true
  ```

5. 安装 `OAI Compatible Provider for Copilot` 插件
6. 按照[教程](https://zhuanlan.zhihu.com/p/1951238505524093197)配置，**如果配置后看不见模型，就重启一下`vscode`**
7. **该方案只支持`chat`类型的功能，不支持 `code completion`，目前官方还未实现代码补全模型自定义。**

![alt](../../image/vscode/copilot_chat.png)
