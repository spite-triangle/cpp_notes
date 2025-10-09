# copilot


# 内网使用

> [!note]
> 该方式可实现在内网中使用自定义的模型，且只支持 `chat` 功能，无法实现 `code complete`

## github copilot chat

1. 安装 `github copilot chat` 插件
2. 找到插件源码路径 `C:\Users\{USER_NAME}\.vscode-insiders\extensions\github.copilot-chat-{VERSION}\dist`
3. 格式化 `extension.js`

```term
triangle@LEARN:~$ npm install -g prettier
triangle@LEARN:~$ prettier --write extension.js
```

4. 写死 `token` 验证，**且需要调大 `expires_at` 配置，防止`token` 过期**

  ```js
  async doAuthFromGitHubTokenOrDevDeviceId(t) {
    this._telemetryService.sendGHTelemetryEvent('auth.new_login');
    let n, o, s;
    if (
      ('githubToken' in t
        ? ((s = t.ghUsername),
          ([n, o] = await Promise.all([
            this.fetchCopilotTokenFromGitHubToken(
              t.githubToken,
            ),
            this.fetchCopilotUserInfo(t.githubToken),
          ])))
        : (n = await this.fetchCopilotTokenFromDevDeviceId(
            t.devDeviceId,
          )),
      !n)
    )
      return (
        this._logService.warn('Failed to get copilot token'),
        this._telemetryService.sendGHTelemetryErrorEvent(
          'auth.request_failed',
        ),
        { kind: 'failure', reason: 'FailedToGetToken' }
      );

  // NOTE - 以下代码可以获取一个真正的 token
  // let a = await iit(n);
  // this._logService.info(JSON.stringify(a));

  // NOTE - 强制将请求修改正确
  // {"status":200,"statusText":"","headers":{},"ok":true};
  n.status = 200;
  n.ok = true;		
  
  // 自定义 token
  let a = JSON.parse(
    `
    {
      "kind": "success",
      "annotations_enabled": false,
      "blackbird_clientside_indexing": false,
      "chat_enabled": true,
      "chat_jetbrains_enabled": false,
      "code_quote_enabled": false,
      "code_review_enabled": false,
      "codesearch": false,
      "copilotignore_enabled": false,
      "endpoints": {
          "api": "https://api.individual.githubcopilot.com",
          "origin-tracker": "https://origin-tracker.individual.githubcopilot.com",
          "proxy": "https://proxy.individual.githubcopilot.com",
          "telemetry": "https://telemetry.individual.githubcopilot.com"
      },
      "expires_at": 2770033513,
      "individual": false,
      "prompt_8k": true,
      "public_suggestions": "disabled",
      "refresh_in": 300,
      "sku": "no_auth_limited_copilot",
      "snippy_load_test_enabled": false,
      "telemetry": "enabled",
      "token": "tid=70b36c9e-ea08-48c2-b28e-0dd535b39982;exp=1760018889;sku=no_auth_limited_copilot;proxy-ep=proxy.individual.githubcopilot.com;st=dotcom;chat=1;malfil=1;agent_mode=1;mcp=1;8kp=1;ip=116.169.127.145;asn=AS4837:914fa65cc7e51895691f92152810a020d990fcdf44a30cb8005dd3278a3cec98",
      "tracking_id": "70b36c9e-ea08-48c2-b28e-0dd535b39982",
      "vsc_electron_fetcher_v2": false,
      "xcode": false,
      "xcode_chat": false,
      "copilot_plan": "no_auth_limited_copilot",
      "username": "unknown",
      "isVscodeTeamMember": false
    }
    `);

    /* ............... */
  }	 
  ```

5. 写死模型配置，**写死的模型配置并不能被使用，只是为了让代码运行通过**

```js
async getAllChatEndpoints() {
  this._logService.info("getAllChatEndpoints");

  // NOTE 获取真正的模型配置
  // let e = await this._modelFetcher.getAllChatModels(),
  // this._logService.info(JSON.stringify(e));

  // 使用预先获取的模型配置
  let e1 = JSON.parse(
    `
    [
      {
        "auto": true,
        "billing": {
            "is_premium": false,
            "multiplier": 0,
            "restricted_to": [
                "free",
                "pro",
                "pro_plus",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gpt-5-mini",
            "limits": {
                "max_context_window_tokens": 264000,
                "max_output_tokens": 64000,
                "max_prompt_tokens": 127988,
                "vision": {
                    "max_prompt_image_size": 3145728,
                    "max_prompt_images": 1,
                    "supported_media_types": [
                        "image/jpeg",
                        "image/png",
                        "image/webp",
                        "image/gif"
                    ]
                }
            },
            "object": "model_capabilities",
            "supports": {
                "parallel_tool_calls": true,
                "streaming": true,
                "structured_outputs": true,
                "tool_calls": true,
                "vision": true
            },
            "tokenizer": "o200k_base",
            "type": "chat"
        },
        "id": "gpt-5-mini",
        "is_chat_default": true,
        "is_chat_fallback": false,
        "model_picker_category": "lightweight",
        "model_picker_enabled": false,
        "name": "GPT-5 mini",
        "object": "model",
        "policy": {
            "state": "enabled",
            "terms": "Enable access to the latest GPT-5 mini model from OpenAI. [Learn more about how GitHub Copilot serves GPT-5 mini](https://gh.io/copilot-openai)."
        },
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "gpt-5-mini"
    },
    {
        "billing": {
            "is_premium": true,
            "multiplier": 1,
            "restricted_to": [
                "pro",
                "pro_plus",
                "max",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gpt-5",
            "limits": {
                "max_context_window_tokens": 264000,
                "max_output_tokens": 64000,
                "max_prompt_tokens": 127988,
                "vision": {
                    "max_prompt_image_size": 3145728,
                    "max_prompt_images": 1,
                    "supported_media_types": [
                        "image/jpeg",
                        "image/png",
                        "image/webp",
                        "image/gif"
                    ]
                }
            },
            "object": "model_capabilities",
            "supports": {
                "parallel_tool_calls": true,
                "streaming": true,
                "structured_outputs": true,
                "tool_calls": true,
                "vision": true
            },
            "tokenizer": "o200k_base",
            "type": "chat"
        },
        "id": "gpt-5",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_category": "versatile",
        "model_picker_enabled": false,
        "name": "GPT-5",
        "object": "model",
        "policy": {
            "state": "enabled",
            "terms": "Enable access to the latest GPT-5 model from OpenAI. [Learn more about how GitHub Copilot serves GPT-5](https://gh.io/copilot-openai)."
        },
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "gpt-5"
    },
    {
        "billing": {
            "is_premium": false,
            "multiplier": 0,
            "restricted_to": [
                "free",
                "pro",
                "pro_plus",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gpt-3.5-turbo",
            "limits": {
                "max_context_window_tokens": 16384,
                "max_output_tokens": 4096,
                "max_prompt_tokens": 13918
            },
            "object": "model_capabilities",
            "supports": {
                "streaming": true,
                "tool_calls": true
            },
            "tokenizer": "cl100k_base",
            "type": "chat"
        },
        "id": "gpt-3.5-turbo",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_enabled": false,
        "name": "GPT 3.5 Turbo",
        "object": "model",
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "gpt-3.5-turbo-0613"
    },
    {
        "billing": {
            "is_premium": false,
            "multiplier": 0,
            "restricted_to": [
                "free",
                "pro",
                "pro_plus",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gpt-3.5-turbo",
            "limits": {
                "max_context_window_tokens": 16384,
                "max_output_tokens": 4096,
                "max_prompt_tokens": 13918
            },
            "object": "model_capabilities",
            "supports": {
                "streaming": true,
                "tool_calls": true
            },
            "tokenizer": "cl100k_base",
            "type": "chat"
        },
        "id": "gpt-3.5-turbo-0613",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_enabled": false,
        "name": "GPT 3.5 Turbo",
        "object": "model",
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "gpt-3.5-turbo-0613"
    },
    {
        "billing": {
            "is_premium": false,
            "multiplier": 0,
            "restricted_to": [
                "free",
                "pro",
                "pro_plus",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gpt-4o-mini",
            "limits": {
                "max_context_window_tokens": 128000,
                "max_output_tokens": 4096,
                "max_prompt_tokens": 63988
            },
            "object": "model_capabilities",
            "supports": {
                "parallel_tool_calls": true,
                "streaming": true,
                "tool_calls": true
            },
            "tokenizer": "o200k_base",
            "type": "chat"
        },
        "id": "gpt-4o-mini",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_enabled": false,
        "name": "GPT-4o mini",
        "object": "model",
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "gpt-4o-mini-2024-07-18"
    },
    {
        "billing": {
            "is_premium": false,
            "multiplier": 0,
            "restricted_to": [
                "free",
                "pro",
                "pro_plus",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gpt-4o-mini",
            "limits": {
                "max_context_window_tokens": 128000,
                "max_output_tokens": 4096,
                "max_prompt_tokens": 63988
            },
            "object": "model_capabilities",
            "supports": {
                "parallel_tool_calls": true,
                "streaming": true,
                "tool_calls": true
            },
            "tokenizer": "o200k_base",
            "type": "chat"
        },
        "id": "gpt-4o-mini-2024-07-18",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_enabled": false,
        "name": "GPT-4o mini",
        "object": "model",
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "gpt-4o-mini-2024-07-18"
    },
    {
        "billing": {
            "is_premium": false,
            "multiplier": 0,
            "restricted_to": [
                "free",
                "pro",
                "pro_plus",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gpt-4",
            "limits": {
                "max_context_window_tokens": 32768,
                "max_output_tokens": 4096,
                "max_prompt_tokens": 28663
            },
            "object": "model_capabilities",
            "supports": {
                "streaming": true,
                "tool_calls": true
            },
            "tokenizer": "cl100k_base",
            "type": "chat"
        },
        "id": "gpt-4",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_enabled": false,
        "name": "GPT 4",
        "object": "model",
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "gpt-4-0613"
    },
    {
        "billing": {
            "is_premium": false,
            "multiplier": 0,
            "restricted_to": [
                "free",
                "pro",
                "pro_plus",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gpt-4",
            "limits": {
                "max_context_window_tokens": 32768,
                "max_output_tokens": 4096,
                "max_prompt_tokens": 28663
            },
            "object": "model_capabilities",
            "supports": {
                "streaming": true,
                "tool_calls": true
            },
            "tokenizer": "cl100k_base",
            "type": "chat"
        },
        "id": "gpt-4-0613",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_enabled": false,
        "name": "GPT 4",
        "object": "model",
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "gpt-4-0613"
    },
    {
        "billing": {
            "is_premium": false,
            "multiplier": 0,
            "restricted_to": [
                "free",
                "pro",
                "pro_plus",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gpt-4o",
            "limits": {
                "max_context_window_tokens": 128000,
                "max_output_tokens": 4096,
                "max_prompt_tokens": 63988,
                "vision": {
                    "max_prompt_image_size": 3145728,
                    "max_prompt_images": 1,
                    "supported_media_types": [
                        "image/jpeg",
                        "image/png",
                        "image/webp",
                        "image/gif"
                    ]
                }
            },
            "object": "model_capabilities",
            "supports": {
                "parallel_tool_calls": true,
                "streaming": true,
                "tool_calls": true,
                "vision": true
            },
            "tokenizer": "o200k_base",
            "type": "chat"
        },
        "id": "gpt-4o",
        "is_chat_default": false,
        "is_chat_fallback": true,
        "model_picker_category": "versatile",
        "model_picker_enabled": false,
        "name": "GPT-4o",
        "object": "model",
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "gpt-4o-2024-11-20"
    },
    {
        "billing": {
            "is_premium": false,
            "multiplier": 0,
            "restricted_to": [
                "free",
                "pro",
                "pro_plus",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gpt-4o",
            "limits": {
                "max_context_window_tokens": 128000,
                "max_output_tokens": 16384,
                "max_prompt_tokens": 63988,
                "vision": {
                    "max_prompt_image_size": 3145728,
                    "max_prompt_images": 1,
                    "supported_media_types": [
                        "image/jpeg",
                        "image/png",
                        "image/webp",
                        "image/gif"
                    ]
                }
            },
            "object": "model_capabilities",
            "supports": {
                "parallel_tool_calls": true,
                "streaming": true,
                "tool_calls": true,
                "vision": true
            },
            "tokenizer": "o200k_base",
            "type": "chat"
        },
        "id": "gpt-4o-2024-11-20",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_enabled": false,
        "name": "GPT-4o",
        "object": "model",
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "gpt-4o-2024-11-20"
    },
    {
        "billing": {
            "is_premium": false,
            "multiplier": 0,
            "restricted_to": [
                "free",
                "pro",
                "pro_plus",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gpt-4o",
            "limits": {
                "max_context_window_tokens": 128000,
                "max_output_tokens": 4096,
                "max_prompt_tokens": 63988,
                "vision": {
                    "max_prompt_image_size": 3145728,
                    "max_prompt_images": 1,
                    "supported_media_types": [
                        "image/jpeg",
                        "image/png",
                        "image/webp",
                        "image/gif"
                    ]
                }
            },
            "object": "model_capabilities",
            "supports": {
                "parallel_tool_calls": true,
                "streaming": true,
                "tool_calls": true,
                "vision": true
            },
            "tokenizer": "o200k_base",
            "type": "chat"
        },
        "id": "gpt-4o-2024-05-13",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_enabled": false,
        "name": "GPT-4o",
        "object": "model",
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "gpt-4o-2024-05-13"
    },
    {
        "billing": {
            "is_premium": false,
            "multiplier": 0,
            "restricted_to": [
                "free",
                "pro",
                "pro_plus",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gpt-4o",
            "limits": {
                "max_context_window_tokens": 128000,
                "max_output_tokens": 4096,
                "max_prompt_tokens": 63988
            },
            "object": "model_capabilities",
            "supports": {
                "parallel_tool_calls": true,
                "streaming": true,
                "tool_calls": true
            },
            "tokenizer": "o200k_base",
            "type": "chat"
        },
        "id": "gpt-4-o-preview",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_enabled": false,
        "name": "GPT-4o",
        "object": "model",
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "gpt-4o-2024-05-13"
    },
    {
        "billing": {
            "is_premium": false,
            "multiplier": 0,
            "restricted_to": [
                "free",
                "pro",
                "pro_plus",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gpt-4o",
            "limits": {
                "max_context_window_tokens": 128000,
                "max_output_tokens": 16384,
                "max_prompt_tokens": 63988
            },
            "object": "model_capabilities",
            "supports": {
                "parallel_tool_calls": true,
                "streaming": true,
                "tool_calls": true
            },
            "tokenizer": "o200k_base",
            "type": "chat"
        },
        "id": "gpt-4o-2024-08-06",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_enabled": false,
        "name": "GPT-4o",
        "object": "model",
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "gpt-4o-2024-08-06"
    },
    {
        "billing": {
            "is_premium": true,
            "multiplier": 0.33,
            "restricted_to": [
                "free",
                "pro",
                "pro_plus",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "o3-mini",
            "limits": {
                "max_context_window_tokens": 200000,
                "max_output_tokens": 100000,
                "max_prompt_tokens": 63988
            },
            "object": "model_capabilities",
            "supports": {
                "streaming": true,
                "structured_outputs": true,
                "tool_calls": true
            },
            "tokenizer": "o200k_base",
            "type": "chat"
        },
        "id": "o3-mini-paygo",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_enabled": false,
        "name": "o3-mini",
        "object": "model",
        "preview": false,
        "vendor": "Azure OpenAI",
        "version": "o3-mini-paygo"
    },
    {
        "billing": {
            "is_premium": false,
            "multiplier": 0,
            "restricted_to": [
                "pro",
                "pro_plus",
                "max",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "grok-code",
            "limits": {
                "max_context_window_tokens": 128000,
                "max_output_tokens": 64000,
                "max_prompt_tokens": 108792
            },
            "object": "model_capabilities",
            "supports": {
                "streaming": true,
                "structured_outputs": true,
                "tool_calls": true
            },
            "tokenizer": "o200k_base",
            "type": "chat"
        },
        "id": "grok-code-fast-1",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_category": "powerful",
        "model_picker_enabled": false,
        "name": "Grok Code Fast 1 (Preview)",
        "object": "model",
        "policy": {
            "state": "enabled",
            "terms": "Enable access to the latest Grok Code Fast 1 model from xAI. If enabled, you instruct GitHub Copilot to send data to xAI Grok Code Fast 1. [Learn more about how GitHub Copilot serves Grok Code Fast 1](https://docs.github.com/en/copilot/reference/ai-models/model-hosting#xai-models). During launch week, [promotional pricing is 0x](https://gh.io/copilot-grok-code-promo)."
        },
        "preview": true,
        "vendor": "xAI",
        "version": "grok-code-fast-1"
    },
    {
        "billing": {
            "is_premium": true,
            "multiplier": 1,
            "restricted_to": [
                "pro",
                "pro_plus",
                "max",
                "business",
                "enterprise"
            ]
        },
        "capabilities": {
            "family": "gemini-2.5-pro",
            "limits": {
                "max_context_window_tokens": 128000,
                "max_output_tokens": 64000,
                "max_prompt_tokens": 108792,
                "vision": {
                    "max_prompt_image_size": 3145728,
                    "max_prompt_images": 1,
                    "supported_media_types": [
                        "image/jpeg",
                        "image/png",
                        "image/webp",
                        "image/heic",
                        "image/heif"
                    ]
                }
            },
            "object": "model_capabilities",
            "supports": {
                "max_thinking_budget": 32768,
                "min_thinking_budget": 128,
                "parallel_tool_calls": true,
                "streaming": true,
                "tool_calls": true,
                "vision": true
            },
            "tokenizer": "o200k_base",
            "type": "chat"
        },
        "id": "gemini-2.5-pro",
        "is_chat_default": false,
        "is_chat_fallback": false,
        "model_picker_category": "powerful",
        "model_picker_enabled": false,
        "name": "Gemini 2.5 Pro",
        "object": "model",
        "policy": {
            "state": "enabled",
            "terms": "Enable access to the latest Gemini 2.5 Pro model from Google. [Learn more about how GitHub Copilot serves Gemini 2.5 Pro](https://docs.github.com/en/copilot/using-github-copilot/ai-models/choosing-the-right-ai-model-for-your-task#gemini-25-pro)."
        },
        "preview": false,
        "vendor": "Google",
        "version": "gemini-2.5-pro"
    }
  ]
    `
  );

  // let e = await this._modelFetcher.getAllChatModels(),
  let e = e1,
    t = [],
    n = Jtt(this._expService);

  /* .............. */
}
```

5. 写死模型会话获取，**需要调大 `expires_at` 自动值，防止会话过期**

```js
async _updateAutoEndpointCache(t, n) {
  let o = Date.now(),
    s = lDe(t),
    a = this._autoModelCache.get(s),
    l = a?.autoModeToken,
    c = a && a.expiration <= Date.now(),
    d = {
      'Content-Type': 'application/json',
      Authorization: `Bearer ${(await this._authService.getCopilotToken()).token}`,
    };
  l && !c && (d['Copilot-Session-Token'] = l);

  // NOTE - 真正请求代码
  // let f = await (
  // 		await this._capiClientService.makeRequest(
  // 			{
  // 				json: { auto_mode: { model_hints: ['auto'] } },
  // 				headers: d,
  // 				method: 'POST',
  // 			},
  // 			{ type: Zr.AutoModels },
  // 		)
  // 	).json(),

  // NOTE - 写死的假逻辑
  let f = JSON.parse(
    `{
      "available_models": [
        "gpt-5-mini"
      ],
      "selected_model": "gpt-5-mini",
      "session_token": "eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9.eyJhdmFpbGFibGVfbW9kZWxzIjpbImdwdC01LW1pbmkiXSwic2VsZWN0ZWRfbW9kZWwiOiJncHQtNS1taW5pIiwic3ViIjoiNzBiMzZjOWUtZWEwOC00OGMyLWIyOGUtMGRkNTM1YjM5OTgyIiwiaWF0IjoxNzYwMDI5OTEzLCJleHAiOjE3NjAwMzM1MTMsImRpc2NvdW50ZWRfY29zdHMiOnsiZ3B0LTUtbWluaSI6MC4xfX0.0ldG2uQkBiuiC7BtHHbFvqFrEsxo50L5Jaai5BZWoRd9M_lX8hWr-waNdx6zQ1qVQUIVnwa-AIw7ENHvPNMvIw",
      "expires_at": 2770033513, 
      "discounted_costs": {
        "gpt-5-mini": 0.1
      }
    }`
  ),
    m = n.find((g) => g.model === f.selected_model) || n[0],
    h = new Gy(
      m,
      this._chatMLFetcher,
      f.session_token,
      f.discounted_costs?.[m.model] || 0,
    );

  return (
    this._autoModelCache.set(s, {
      endpoint: h,
      expiration: f.expires_at * 1e3,
      autoModeToken: f.session_token,
      lastRequestId: t?.id,
    }),
    this._logService.info(
      `Fetched auto model in ${Date.now() - o}ms.`,
    ),
    h
  );
}
```

7. 写死 `_fetchModels`，这样才能正确调用模型

```js
async _fetchModels(t) {
		if (!t && !this._shouldRefreshModels()) return;
		let n = Date.now(),
			o = (await this._authService.getCopilotToken()).token,
			s = wt(),
			a = { type: Zr.Models, isModelLab: this._isModelLab };
		try {

      // NOTE - 网络请求全部注释掉
			// let l = await QC(
			// 	this._fetcher,
			// 	this._telemetryService,
			// 	this._capiClientService,
			// 	a,
			// 	o,
			// 	await fc(process.env.HMAC_SECRET),
			// 	'model-access',
			// 	s,
			// );
			// if (
			// 	((this._lastFetchTime = Date.now()),
			// 	this._logService.info(
			// 		`Fetched model metadata in ${Date.now() - n}ms ${s}`,
			// 	),
			// 	l.status < 200 || l.status >= 300)
			// ) {
			// 	if (l.status === 429 && this._familyMap.size > 0) {
			// 		this._logService.warn(
			// 			`Rate limited while fetching models ${s}`,
			// 		);
			// 		return;
			// 	}
			// 	throw new Error(
			// 		`Failed to fetch models (${s}): ${(await l.text()) || l.statusText || `HTTP ${l.status}`}`,
			// 	);
			// }
			this._familyMap.clear();
			// let js = await l.json();
			// this._logService.info(JSON.stringify(js));
			let js = JSON.parse(
				`
				{
    "data": [
        {
            "auto": true,
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-5-mini",
                "limits": {
                    "max_context_window_tokens": 264000,
                    "max_output_tokens": 64000,
                    "max_prompt_tokens": 128000,
                    "vision": {
                        "max_prompt_image_size": 3145728,
                        "max_prompt_images": 1,
                        "supported_media_types": [
                            "image/jpeg",
                            "image/png",
                            "image/webp",
                            "image/gif"
                        ]
                    }
                },
                "object": "model_capabilities",
                "supports": {
                    "parallel_tool_calls": true,
                    "streaming": true,
                    "structured_outputs": true,
                    "tool_calls": true,
                    "vision": true
                },
                "tokenizer": "o200k_base",
                "type": "chat"
            },
            "id": "gpt-5-mini",
            "is_chat_default": true,
            "is_chat_fallback": false,
            "model_picker_category": "lightweight",
            "model_picker_enabled": false,
            "name": "GPT-5 mini",
            "object": "model",
            "policy": {
                "state": "enabled",
                "terms": "Enable access to the latest GPT-5 mini model from OpenAI. [Learn more about how GitHub Copilot serves GPT-5 mini](https://gh.io/copilot-openai)."
            },
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-5-mini"
        },
        {
            "billing": {
                "is_premium": true,
                "multiplier": 1,
                "restricted_to": [
                    "pro",
                    "pro_plus",
                    "max",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-5",
                "limits": {
                    "max_context_window_tokens": 264000,
                    "max_output_tokens": 64000,
                    "max_prompt_tokens": 128000,
                    "vision": {
                        "max_prompt_image_size": 3145728,
                        "max_prompt_images": 1,
                        "supported_media_types": [
                            "image/jpeg",
                            "image/png",
                            "image/webp",
                            "image/gif"
                        ]
                    }
                },
                "object": "model_capabilities",
                "supports": {
                    "parallel_tool_calls": true,
                    "streaming": true,
                    "structured_outputs": true,
                    "tool_calls": true,
                    "vision": true
                },
                "tokenizer": "o200k_base",
                "type": "chat"
            },
            "id": "gpt-5",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_category": "versatile",
            "model_picker_enabled": false,
            "name": "GPT-5",
            "object": "model",
            "policy": {
                "state": "enabled",
                "terms": "Enable access to the latest GPT-5 model from OpenAI. [Learn more about how GitHub Copilot serves GPT-5](https://gh.io/copilot-openai)."
            },
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-5"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-3.5-turbo",
                "limits": {
                    "max_context_window_tokens": 16384,
                    "max_output_tokens": 4096,
                    "max_prompt_tokens": 16384
                },
                "object": "model_capabilities",
                "supports": {
                    "streaming": true,
                    "tool_calls": true
                },
                "tokenizer": "cl100k_base",
                "type": "chat"
            },
            "id": "gpt-3.5-turbo",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "GPT 3.5 Turbo",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-3.5-turbo-0613"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-3.5-turbo",
                "limits": {
                    "max_context_window_tokens": 16384,
                    "max_output_tokens": 4096,
                    "max_prompt_tokens": 16384
                },
                "object": "model_capabilities",
                "supports": {
                    "streaming": true,
                    "tool_calls": true
                },
                "tokenizer": "cl100k_base",
                "type": "chat"
            },
            "id": "gpt-3.5-turbo-0613",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "GPT 3.5 Turbo",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-3.5-turbo-0613"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-4o-mini",
                "limits": {
                    "max_context_window_tokens": 128000,
                    "max_output_tokens": 4096,
                    "max_prompt_tokens": 64000
                },
                "object": "model_capabilities",
                "supports": {
                    "parallel_tool_calls": true,
                    "streaming": true,
                    "tool_calls": true
                },
                "tokenizer": "o200k_base",
                "type": "chat"
            },
            "id": "gpt-4o-mini",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "GPT-4o mini",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-4o-mini-2024-07-18"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-4o-mini",
                "limits": {
                    "max_context_window_tokens": 128000,
                    "max_output_tokens": 4096,
                    "max_prompt_tokens": 64000
                },
                "object": "model_capabilities",
                "supports": {
                    "parallel_tool_calls": true,
                    "streaming": true,
                    "tool_calls": true
                },
                "tokenizer": "o200k_base",
                "type": "chat"
            },
            "id": "gpt-4o-mini-2024-07-18",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "GPT-4o mini",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-4o-mini-2024-07-18"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-4",
                "limits": {
                    "max_context_window_tokens": 32768,
                    "max_output_tokens": 4096,
                    "max_prompt_tokens": 32768
                },
                "object": "model_capabilities",
                "supports": {
                    "streaming": true,
                    "tool_calls": true
                },
                "tokenizer": "cl100k_base",
                "type": "chat"
            },
            "id": "gpt-4",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "GPT 4",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-4-0613"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-4",
                "limits": {
                    "max_context_window_tokens": 32768,
                    "max_output_tokens": 4096,
                    "max_prompt_tokens": 32768
                },
                "object": "model_capabilities",
                "supports": {
                    "streaming": true,
                    "tool_calls": true
                },
                "tokenizer": "cl100k_base",
                "type": "chat"
            },
            "id": "gpt-4-0613",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "GPT 4",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-4-0613"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-4o",
                "limits": {
                    "max_context_window_tokens": 128000,
                    "max_output_tokens": 4096,
                    "max_prompt_tokens": 64000,
                    "vision": {
                        "max_prompt_image_size": 3145728,
                        "max_prompt_images": 1,
                        "supported_media_types": [
                            "image/jpeg",
                            "image/png",
                            "image/webp",
                            "image/gif"
                        ]
                    }
                },
                "object": "model_capabilities",
                "supports": {
                    "parallel_tool_calls": true,
                    "streaming": true,
                    "tool_calls": true,
                    "vision": true
                },
                "tokenizer": "o200k_base",
                "type": "chat"
            },
            "id": "gpt-4o",
            "is_chat_default": false,
            "is_chat_fallback": true,
            "model_picker_category": "versatile",
            "model_picker_enabled": false,
            "name": "GPT-4o",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-4o-2024-11-20"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-4o",
                "limits": {
                    "max_context_window_tokens": 128000,
                    "max_output_tokens": 16384,
                    "max_prompt_tokens": 64000,
                    "vision": {
                        "max_prompt_image_size": 3145728,
                        "max_prompt_images": 1,
                        "supported_media_types": [
                            "image/jpeg",
                            "image/png",
                            "image/webp",
                            "image/gif"
                        ]
                    }
                },
                "object": "model_capabilities",
                "supports": {
                    "parallel_tool_calls": true,
                    "streaming": true,
                    "tool_calls": true,
                    "vision": true
                },
                "tokenizer": "o200k_base",
                "type": "chat"
            },
            "id": "gpt-4o-2024-11-20",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "GPT-4o",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-4o-2024-11-20"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-4o",
                "limits": {
                    "max_context_window_tokens": 128000,
                    "max_output_tokens": 4096,
                    "max_prompt_tokens": 64000,
                    "vision": {
                        "max_prompt_image_size": 3145728,
                        "max_prompt_images": 1,
                        "supported_media_types": [
                            "image/jpeg",
                            "image/png",
                            "image/webp",
                            "image/gif"
                        ]
                    }
                },
                "object": "model_capabilities",
                "supports": {
                    "parallel_tool_calls": true,
                    "streaming": true,
                    "tool_calls": true,
                    "vision": true
                },
                "tokenizer": "o200k_base",
                "type": "chat"
            },
            "id": "gpt-4o-2024-05-13",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "GPT-4o",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-4o-2024-05-13"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-4o",
                "limits": {
                    "max_context_window_tokens": 128000,
                    "max_output_tokens": 4096,
                    "max_prompt_tokens": 64000
                },
                "object": "model_capabilities",
                "supports": {
                    "parallel_tool_calls": true,
                    "streaming": true,
                    "tool_calls": true
                },
                "tokenizer": "o200k_base",
                "type": "chat"
            },
            "id": "gpt-4-o-preview",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "GPT-4o",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-4o-2024-05-13"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-4o",
                "limits": {
                    "max_context_window_tokens": 128000,
                    "max_output_tokens": 16384,
                    "max_prompt_tokens": 64000
                },
                "object": "model_capabilities",
                "supports": {
                    "parallel_tool_calls": true,
                    "streaming": true,
                    "tool_calls": true
                },
                "tokenizer": "o200k_base",
                "type": "chat"
            },
            "id": "gpt-4o-2024-08-06",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "GPT-4o",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-4o-2024-08-06"
        },
        {
            "billing": {
                "is_premium": true,
                "multiplier": 0.33,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "o3-mini",
                "limits": {
                    "max_context_window_tokens": 200000,
                    "max_output_tokens": 100000,
                    "max_prompt_tokens": 64000
                },
                "object": "model_capabilities",
                "supports": {
                    "streaming": true,
                    "structured_outputs": true,
                    "tool_calls": true
                },
                "tokenizer": "o200k_base",
                "type": "chat"
            },
            "id": "o3-mini-paygo",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "o3-mini",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "o3-mini-paygo"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gpt-4.1",
                "object": "model_capabilities",
                "supports": {
                    "streaming": true
                },
                "tokenizer": "o200k_base",
                "type": "completion"
            },
            "id": "gpt-41-copilot",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_category": "versatile",
            "model_picker_enabled": false,
            "name": "GPT-4.1 Copilot",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "gpt-41-copilot"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "pro",
                    "pro_plus",
                    "max",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "grok-code",
                "limits": {
                    "max_context_window_tokens": 128000,
                    "max_output_tokens": 64000,
                    "max_prompt_tokens": 128000
                },
                "object": "model_capabilities",
                "supports": {
                    "streaming": true,
                    "structured_outputs": true,
                    "tool_calls": true
                },
                "tokenizer": "o200k_base",
                "type": "chat"
            },
            "id": "grok-code-fast-1",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_category": "powerful",
            "model_picker_enabled": false,
            "name": "Grok Code Fast 1 (Preview)",
            "object": "model",
            "policy": {
                "state": "enabled",
                "terms": "Enable access to the latest Grok Code Fast 1 model from xAI. If enabled, you instruct GitHub Copilot to send data to xAI Grok Code Fast 1. [Learn more about how GitHub Copilot serves Grok Code Fast 1](https://docs.github.com/en/copilot/reference/ai-models/model-hosting#xai-models). During launch week, [promotional pricing is 0x](https://gh.io/copilot-grok-code-promo)."
            },
            "preview": true,
            "vendor": "xAI",
            "version": "grok-code-fast-1"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "text-embedding-ada-002",
                "limits": {
                    "max_inputs": 512
                },
                "object": "model_capabilities",
                "supports": {},
                "tokenizer": "cl100k_base",
                "type": "embeddings"
            },
            "id": "text-embedding-ada-002",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "Embedding V2 Ada",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "text-embedding-3-small"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "text-embedding-3-small",
                "limits": {
                    "max_inputs": 512
                },
                "object": "model_capabilities",
                "supports": {
                    "dimensions": true
                },
                "tokenizer": "cl100k_base",
                "type": "embeddings"
            },
            "id": "text-embedding-3-small",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "Embedding V3 small",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "text-embedding-3-small"
        },
        {
            "billing": {
                "is_premium": false,
                "multiplier": 0,
                "restricted_to": [
                    "free",
                    "pro",
                    "pro_plus",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "text-embedding-3-small",
                "object": "model_capabilities",
                "supports": {
                    "dimensions": true
                },
                "tokenizer": "cl100k_base",
                "type": "embeddings"
            },
            "id": "text-embedding-3-small-inference",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_enabled": false,
            "name": "Embedding V3 small (Inference)",
            "object": "model",
            "preview": false,
            "vendor": "Azure OpenAI",
            "version": "text-embedding-3-small"
        },
        {
            "billing": {
                "is_premium": true,
                "multiplier": 1,
                "restricted_to": [
                    "pro",
                    "pro_plus",
                    "max",
                    "business",
                    "enterprise"
                ]
            },
            "capabilities": {
                "family": "gemini-2.5-pro",
                "limits": {
                    "max_context_window_tokens": 128000,
                    "max_output_tokens": 64000,
                    "max_prompt_tokens": 128000,
                    "vision": {
                        "max_prompt_image_size": 3145728,
                        "max_prompt_images": 1,
                        "supported_media_types": [
                            "image/jpeg",
                            "image/png",
                            "image/webp",
                            "image/heic",
                            "image/heif"
                        ]
                    }
                },
                "object": "model_capabilities",
                "supports": {
                    "max_thinking_budget": 32768,
                    "min_thinking_budget": 128,
                    "parallel_tool_calls": true,
                    "streaming": true,
                    "tool_calls": true,
                    "vision": true
                },
                "tokenizer": "o200k_base",
                "type": "chat"
            },
            "id": "gemini-2.5-pro",
            "is_chat_default": false,
            "is_chat_fallback": false,
            "model_picker_category": "powerful",
            "model_picker_enabled": false,
            "name": "Gemini 2.5 Pro",
            "object": "model",
            "policy": {
                "state": "enabled",
                "terms": "Enable access to the latest Gemini 2.5 Pro model from Google. [Learn more about how GitHub Copilot serves Gemini 2.5 Pro](https://docs.github.com/en/copilot/using-github-copilot/ai-models/choosing-the-right-ai-model-for-your-task#gemini-25-pro)."
            },
            "preview": false,
            "vendor": "Google",
            "version": "gemini-2.5-pro"
        }
    ],
    "object": "list"
    }`
			);
			let c = js.data;
			this._requestLogger.logModelListCall(s, a, c);

      /* .... */
  }
}
```

##  vscode

1. 下载并安装 [insider](https://code.visualstudio.com/insiders/) 版本
2. 切换到 `${install_folder}/Microsoft VS Code Insiders/resources/app/out/vs/workbench` 文件夹

```term
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
