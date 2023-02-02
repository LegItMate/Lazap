<template>
  <div class="leftbar">
    <div class="d-flex justify-content-center">
      <div>
        <div class="user-pfp">
          <label for="file"></label>
          <input
            id="file"
            type="file"
            accept="image/png"
            @change="(event) => loadPFP(event)"
          />
          <img
            src="../assets/svg/default-profile.svg"
            alt="Avatar"
            width="88"
            id="output"
          />
        </div>
        <input
          class="username"
          id="text"
          type="text"
          value="Lazap"
          spellcheck="false"
          maxlength="12"
        />
      </div>
    </div>

    <div class="side-tabs">
      <div class="d-flex justify-content-center pb-2">
        <div class="side-tab" id="home-btn">
          <div class="indicator" id="indicator"></div>
          <img src="../assets/svg/home.svg" height="25" width="25" />
          <div class="side-tab-text">Home</div>
        </div>
      </div>

      <div class="category-name d-flex justify-content-left">Games</div>
      <div class="d-flex justify-content-center">
        <div class="side-tab" id="recent-btn">
          <img
            id="recently-btn-img"
            src="../assets/svg/recent.svg"
            height="25"
            width="25"
          />
          <div class="side-tab-text">Recent</div>
        </div>
      </div>
      <div class="d-flex justify-content-center">
        <div class="side-tab" id="games-btn">
          <img
            id="games-btn-img"
            src="../assets/svg/games.svg"
            height="25"
            width="25"
          />
          <div class="side-tab-text">All Games</div>
        </div>
      </div>
      <div class="d-flex justify-content-center pb-2">
        <div class="side-tab" id="favs-btn">
          <img
            id="favs-btn-img"
            src="../assets/svg/favs.svg"
            height="25"
            width="25"
          />
          <div class="side-tab-text">Favourites</div>
        </div>
      </div>

      <div class="category-name d-flex justify-content-start">Friends</div>
      <div class="d-flex justify-content-center">
        <div class="side-tab" id="messages-btn">
          <img src="../assets/svg/messages.svg" height="25" width="25" />
          <div class="side-tab-text">Messages</div>
        </div>
      </div>
      <div class="d-flex justify-content-center">
        <div class="side-tab" id="activity-btn">
          <img src="../assets/svg/activity.svg" height="25" width="25" />
          <div class="side-tab-text">Activity</div>
        </div>
      </div>
      <div class="d-flex justify-content-center">
        <div class="side-tab" id="friends-btn">
          <img src="../assets/svg/friends.svg" height="25" width="25" />
          <div class="side-tab-text">All Friends</div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
const invoke = window.__TAURI__.invoke;
const path = window.__TAURI__.path;

export default {
  name: "leftbar-comp",
  methods: {
    async loadPFP(event) {
      let selectedFile = event.target.files[0];
      let reader = new FileReader();

      reader.onload = async function () {
        await invoke("write_binary_file", {
          filePath: (await path.appDir()) + `cache/user/pfp.png`,
          fileContent: [...new Uint8Array(reader.result)],
        });
        document.getElementById("output").src =
          window.__TAURI__.tauri.convertFileSrc(
            (await path.appDir()) + `cache/user/pfp.png`
          ) + `?${new Date().getSeconds()}`;
      };
      reader.readAsArrayBuffer(selectedFile);
    },
  },
  async mounted() {
    let timestamp = null;
    const home = document.getElementById("home");
    const recent = document.getElementById("recent");
    const games = document.getElementById("games");
    const favs = document.getElementById("favs");
    const friends = document.getElementById("friends");
    const messages = document.getElementById("messages");
    const activity = document.getElementById("activity");
    const gameMenu = document.getElementById("gameMenu");
    const findGamesModule = this.$root.$refs.findGamesMod;

    const data = JSON.parse(
      await invoke("read_file", {
        filePath: (await path.appDir()) + "cache/user/data.json",
      })
    );
    document.getElementById("text").value = data.username;

    if (
      await invoke("d_f_exists", {
        path: (await path.appDir()) + "cache/user/pfp.png",
      })
    ) {
      document.getElementById("output").src =
        window.__TAURI__.tauri.convertFileSrc(
          (await path.appDir()) + "cache/user/pfp.png"
        ) + `?${new Date().getSeconds()}`;
    }

    document
      .getElementById("home-btn")
      .addEventListener("click", async function () {
        this.appendChild(document.getElementById("indicator"));
        if (home.style.display !== "flex") {
          toggleIndicatorAnim();
        }

        home.style.display = "flex";
        recent.style.display = "none";
        games.style.display = "none";
        favs.style.display = "none";
        friends.style.display = "none";
        messages.style.display = "none";
        activity.style.display = "none";
        gameMenu.style.display = "none";

        await findGamesModule
          .loadGames("recentGamesListMainPage")
          .catch((err) => {
            return console.error(err);
          });

        setActivity("home");
      });

    document
      .getElementById("recent-btn")
      .addEventListener("click", async function () {
        this.appendChild(document.getElementById("indicator"));
        if (recent.style.display !== "flex") {
          toggleIndicatorAnim();
        }

        home.style.display = "none";
        recent.style.display = "flex";
        games.style.display = "none";
        favs.style.display = "none";
        friends.style.display = "none";
        messages.style.display = "none";
        activity.style.display = "none";
        gameMenu.style.display = "none";

        await findGamesModule.loadGames("recentGamesList").catch((err) => {
          return console.error(err);
        });

        setActivity("recent");
      });

    document
      .getElementById("games-btn")
      .addEventListener("click", async function () {
        this.appendChild(document.getElementById("indicator"));

        if (games.style.display !== "flex") {
          setTimeout(() => document.getElementById("gamesInput").focus(), 100);
          toggleIndicatorAnim();
        }

        home.style.display = "none";
        recent.style.display = "none";
        games.style.display = "flex";
        favs.style.display = "none";
        friends.style.display = "none";
        messages.style.display = "none";
        activity.style.display = "none";
        gameMenu.style.display = "none";

        await findGamesModule.loadGames("allGamesList").catch((err) => {
          return console.error(err);
        });

        setActivity("games");
      });

    document
      .getElementById("favs-btn")
      .addEventListener("click", async function () {
        this.appendChild(document.getElementById("indicator"));

        if (favs.style.display !== "flex") {
          setTimeout(() => document.getElementById("favsInput").focus(), 100);
          toggleIndicatorAnim();
        }

        home.style.display = "none";
        recent.style.display = "none";
        games.style.display = "none";
        favs.style.display = "flex";
        messages.style.display = "none";
        activity.style.display = "none";
        friends.style.display = "none";
        gameMenu.style.display = "none";

        await findGamesModule.loadGames("favGamesList").catch((err) => {
          return console.error(err);
        });

        setActivity("favourites");
      });

    document
      .getElementById("messages-btn")
      .addEventListener("click", async function () {
        this.appendChild(document.getElementById("indicator"));
        if (messages.style.display !== "flex") {
          toggleIndicatorAnim();
        }

        home.style.display = "none";
        recent.style.display = "none";
        games.style.display = "none";
        favs.style.display = "none";
        messages.style.display = "flex";
        activity.style.display = "none";
        friends.style.display = "none";
        gameMenu.style.display = "none";

        setActivity("messages");
      });

    document
      .getElementById("activity-btn")
      .addEventListener("click", async function () {
        this.appendChild(document.getElementById("indicator"));
        if (activity.style.display !== "flex") {
          toggleIndicatorAnim();
        }

        home.style.display = "none";
        recent.style.display = "none";
        games.style.display = "none";
        favs.style.display = "none";
        messages.style.display = "none";
        activity.style.display = "flex";
        friends.style.display = "none";
        gameMenu.style.display = "none";

        setActivity("activity");
      });

    document
      .getElementById("friends-btn")
      .addEventListener("click", async function () {
        this.appendChild(document.getElementById("indicator"));
        if (friends.style.display !== "flex") {
          toggleIndicatorAnim();
        }

        home.style.display = "none";
        recent.style.display = "none";
        games.style.display = "none";
        favs.style.display = "none";
        messages.style.display = "none";
        activity.style.display = "none";
        friends.style.display = "flex";
        gameMenu.style.display = "none";

        setActivity("friends");
      });

    async function setActivity(tab) {
      const { state, details, largeImage, largeText, smallImage, smallText } =
        require("./modules/rpcOptions").selectOption(tab);
      if (timestamp === null) timestamp = Date.now();
      try {
        await invoke(`set_activity`, {
          state,
          details,
          largeImage,
          largeText,
          smallImage,
          smallText,
          timestamp: timestamp === null ? Date.now() : timestamp,
        });
      } catch (error) {
        console.error(error);
      }
    }

    function toggleIndicatorAnim() {
      let indicator = document.getElementById("indicator");
      indicator.classList.add("anim-indicatorscaleY");
      setTimeout(() => {
        indicator.classList.remove("anim-indicatorscaleY");
      }, 200);
    }
  },
};
</script>

<style>
.leftbar {
  background-color: #1c1d22;
  border-radius: 15px;
  position: absolute;
  height: calc(100% - 71px);
  max-height: 100%;
  width: 270px;
  top: 25px;
  margin-top: 30px;
  margin-right: 100%;
  margin-left: 300px;
  text-align: center;
  box-shadow: 0 3px 16px -7px rgb(17 18 24 / 70%);
}

.user-pfp {
  position: relative;
  margin-top: 50px;
  width: 88px;
  height: 88px;
  margin-left: auto;
  margin-right: auto;
  transition: all 0.2s ease-in-out;
}

.user-pfp:hover {
  cursor: pointer;
  transition: all 0.2s ease-in-out;
  border-radius: 14px;
  scale: 1.1;
}

.user-pfp input {
  display: none;
}

.user-pfp img {
  border-radius: 18px;
  image-rendering: auto;
  width: 88px;
  height: 88px;
  object-fit: cover;
  box-shadow: 0 3px 16px -7px rgb(17 18 24 / 70%);
}

.user-pfp label {
  position: absolute;
  cursor: pointer;
  height: 100%;
  width: 100%;
}

.username {
  margin-top: 8px;
  margin-bottom: 26px;
  text-align: center;
  font-size: 22px;
  font-family: Nunito-Bold;
  color: #b9b9b9;
  background-color: transparent;
  border: transparent;
  text-decoration: none;
}

.side-tabs {
  margin-left: 20px;
  position: absolute;
  height: 66%;
  overflow-y: auto;
  overflow-x: hidden;
}

.side-tab {
  background-color: rgba(30, 30, 34, 0.5);
  box-shadow: -1px 3px 8px -1px rgba(0, 0, 0, 0.2);
  height: 40px;
  width: 230px;
  border-radius: 10px;
  display: flex;
  margin-bottom: 10px;
  z-index: 1;
  transition: color 0.15s ease-out;
}

.side-tab:before {
  background: rgb(30, 30, 34);
  background: linear-gradient(
    40deg,
    rgba(30, 30, 34, 1) 0%,
    rgba(30, 30, 34, 1) 30%,
    var(--back) 100%
  );
  content: "";
  height: 40px;
  width: 230px;
  border-radius: 10px;
  display: flex;
  margin-bottom: 10px;
  position: absolute;
  z-index: -1;
  opacity: 0;
  transition: opacity 0.15s ease-out;
}

.side-tab:hover:before {
  opacity: 1;
}

.side-tab:hover {
  cursor: default;
  color: #fff;
}

.side-tab img {
  filter: invert(100%) sepia(6%) saturate(0%) hue-rotate(115deg)
    brightness(108%) contrast(108%);
  margin-left: 25px;
  margin-top: 8px;
  margin-right: 5px;
  display: inline-block;
}

.side-tab .side-tab-text {
  color: #dadada;
  font-size: 18px;
  margin-left: 16px;
  align-self: center;
  margin-top: 4px;
  cursor: default;
}

.category-name {
  color: #7a7a7a;
  margin-left: 10px;
  margin-bottom: 4px;
  font-size: 15px;
}

.category-name:hover {
  cursor: default;
}

.indicator {
  transition: 0.3s;
  position: absolute;
  margin-top: 5px;
  margin-left: 2px;

  background-color: var(--back);
  height: 32px;
  width: 5px;
  border-radius: 50px;
  display: inline;
}

.anim-indicatorscaleY {
  animation: indicatorscaleY 0.2s linear;
}
</style>